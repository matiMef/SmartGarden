using Garden.Api.Data;
using Garden.Api.Models;
using Microsoft.EntityFrameworkCore;
using System.Net.Http.Json;
using System.Text.Json;

namespace Garden.Api.Services
{
    public class DataGeneratorService : BackgroundService
    {
        private readonly IServiceProvider _serviceProvider;
        private readonly HttpClient _httpClient;

        public DataGeneratorService(IServiceProvider serviceProvider)
        {
            _serviceProvider = serviceProvider;
            _httpClient = new HttpClient();
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            string arduinoIp = "192.168.10.124"; // IP Arduino
            string url = $"http://{arduinoIp}/data";

            while (!stoppingToken.IsCancellationRequested)
            {
                try
                {
                    var localZone = TimeZoneInfo.FindSystemTimeZoneById("Europe/Warsaw");
                    var localNow = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, localZone);

                    var response = await _httpClient.GetAsync(url, stoppingToken);
                    if (!response.IsSuccessStatusCode)
                    {
                        Console.WriteLine($"Error while connecting to Arduino: {response.StatusCode}");
                    }
                    else
                    {
                        var content = await response.Content.ReadAsStringAsync(stoppingToken);
                        var data = JsonSerializer.Deserialize<ArduinoData>(content);

                        if (data != null)
                        {
                            using var scope = _serviceProvider.CreateScope();
                            var db = scope.ServiceProvider.GetRequiredService<AppDbContext>();

                            var readings = new List<SensorReadings>
                            {
                                new() { SensorId = 0, Date = DateTime.UtcNow, Value = data.temperature },
                                new() { SensorId = 1, Date = DateTime.UtcNow, Value = data.humidity },
                                new() { SensorId = 2, Date = DateTime.UtcNow, Value = data.soil },
                                new() { SensorId = 3, Date = DateTime.UtcNow, Value = data.lux }
                            };

                            db.SensorReadings.AddRange(readings);
                            await db.SaveChangesAsync(stoppingToken);

                            Console.WriteLine($"Arduino Data Saved {localNow}");
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error while connecting to Arduino: {ex.Message}");
                }

                await Task.Delay(TimeSpan.FromMinutes(10), stoppingToken);
            }
        }   
    }
}
