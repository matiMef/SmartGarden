using Microsoft.AspNetCore.Mvc;
using System.Net.Http;
using System.Text.Json;
using System.Threading.Tasks;
using Garden.Api.Models;

namespace Garden.Api.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ArduinoController : ControllerBase
    {
        private readonly HttpClient _httpClient;

        public ArduinoController(HttpClient httpClient)
        {
            _httpClient = httpClient;
        }

        [HttpGet("getData")]
        public async Task<IActionResult> GetArduinoData()
        {
            // Arduino IP address
            // Endpoint to get data from Arduino
            string arduinoIp = "192.168.4.1";
            string url = $"http://{arduinoIp}/data";

            // Send GET request to Arduino
            // Return the data from sensors as JSON response
            try
            {
                // Connect to Arduino and get data
                var response = await _httpClient.GetAsync(url);
                if (!response.IsSuccessStatusCode)
                {
                    return StatusCode((int)response.StatusCode, "Error while connecting to Arduino");
                }

                var content = await response.Content.ReadAsStringAsync();

                // Dynamic Json deserialization
                var data = JsonSerializer.Deserialize<JsonElement>(content);

                return Ok(data);
            }
            catch (Exception ex)
            {
                return StatusCode(500, $"Error: {ex.Message}");
            }
        }

        // Send command to Arduino
        // Return status of the command
        [HttpPost("sendCommand")]
        public async Task<IActionResult> SendCommandToArduino([FromBody] ArduinoPayload data)
        {
            string arduinoIp = "192.168.4.1";
            string url = $"http://{arduinoIp}/command";

            try
            {
                // Example payload to send
                var dataToSend = new
                {
                    payload = "start_short"
                };

                Console.WriteLine(data.payload);
                
                // Send POST request to Arduino with command
                var response = await _httpClient.PostAsJsonAsync(url, data);
                if (!response.IsSuccessStatusCode)
                {
                    return StatusCode((int)response.StatusCode, "Error while sending command to Arduino");
                }

                // Check for specific response indicating low water level
                string responseText = await response.Content.ReadAsStringAsync();
                if (responseText.Contains("LOW_WATER_LEVEL", StringComparison.OrdinalIgnoreCase))
                {
                    return Ok("LOW_WATER_LEVEL");
                }
                if (responseText.Contains("WATERING_ACTIVE", StringComparison.OrdinalIgnoreCase))
                {
                    return Ok("WATERING_ACTIVE");
                }
                if (responseText.Contains("OK", StringComparison.OrdinalIgnoreCase))
                {
                    return Ok("WATERING_STARTED");
                }


                return Ok("Command sent successfully");
            }
            catch(Exception ex)
            {
                return StatusCode(500, $"Error {ex.Message}");
            }
        }
    }
}
