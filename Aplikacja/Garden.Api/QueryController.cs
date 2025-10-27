using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using System.Text.Json;
using System.Threading.Tasks;
using Garden.Api.Data;
using Garden.Api.Models;

namespace Garden.Api.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class QueryController : ControllerBase // React prefers ControllerBase over Controller
    {
        private readonly AppDbContext _context;
        public QueryController(AppDbContext context)
        {
            _context = context;
        }

        [HttpGet("SoilDataToday")]
        public async Task<IActionResult> GetSoilDataToday()
        {   
            var localZone = TimeZoneInfo.FindSystemTimeZoneById("Central European Standard Time");
            var localTimeNow = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, localZone);
            var todayLocal = localTimeNow.Date;
            var tomorrowLocal = todayLocal.AddDays(1);

            var startUtc = TimeZoneInfo.ConvertTimeToUtc(todayLocal, localZone);
            var endUtc = TimeZoneInfo.ConvertTimeToUtc(tomorrowLocal, localZone);

            try
            {

                var rawReadings = await _context.SensorReadings.Where(reading => reading.SensorId == 2)
                    .Where(reading => reading.Date >= startUtc && reading.Date < endUtc)/*adding today to query*/
                    .ToListAsync();
                    
                var readings = rawReadings
                    .GroupBy( reading =>
                    {
                        var localTime = TimeZoneInfo.ConvertTimeFromUtc(reading.Date, localZone);
                        return new DateTime(localTime.Year, localTime.Month, localTime.Day, localTime.Hour, 0, 0);
                    })
                    .Select(group => new
                    {
                        Date = group.Key,
                        // Average soil moisture for the day
                        // Should be rounded to 2 decimal places in React !!!
                        Soil = group.Average(reading => (reading.Value))
                    }).OrderBy(reading => reading.Date).ToList();

                return Ok(readings);
            }
            catch (Exception ex)
            {
                return StatusCode(500, $"Error: {ex.Message}");
            }
        }

        [HttpGet("SoilDataWeek")]
        public async Task<IActionResult> GetSoilDataWeek()
        {
            var localZone = TimeZoneInfo.FindSystemTimeZoneById("Central European Standard Time");
            var nowLocal = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, localZone);
            
            var todayLocal = nowLocal.Date;
            var startLocal = todayLocal.AddDays(-6);
           
            var endUtc = TimeZoneInfo.ConvertTimeToUtc(todayLocal, localZone);
            var startUtc = TimeZoneInfo.ConvertTimeToUtc(startLocal, localZone);
           
            try
            {
                var rawReadings = await _context.SensorReadings.Where(reading => reading.SensorId == 2)
                    .Where(reading => reading.Date >= startUtc && reading.Date <= endUtc.AddDays(1))/*adding today to query*/
                    .ToListAsync();
                    
                var readings = rawReadings
                    .GroupBy(reading =>
                    {
                        var localDate = TimeZoneInfo.ConvertTimeFromUtc(reading.Date, localZone);
                        return localDate.Date;
                    })
                    .Select(group => new
                    {
                        Date = group.Key,
                        // Average soil moisture for the day
                        // Should be rounded to 2 decimal places in React !!!
                        Soil = group.Average(reading => (reading.Value))
                    })
                    .OrderBy(reading => reading.Date)
                    .ToList();

                return Ok(readings);
            }
            catch (Exception ex)
            {
                return StatusCode(500, $"Error: {ex.Message}");
            }
        }

        [HttpGet("SoilDataLastMonth")]
        public async Task<IActionResult> GetSoilDataLastMonth()
        {
            // Hours may differ if server is in different time zone than user

            var localZone = TimeZoneInfo.FindSystemTimeZoneById("Central European Standard Time");
            var localDate = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, localZone);
            
            //var localMonth = localDate.Month;
            //var localYear = localDate.Year;
            //var fixedMonth = (localMonth == 1) ? 12 : localMonth - 1;
            //var fixedYear = (localMonth == 1) ? localYear - 1 : localYear;
            //var startLocal = new DateTime(fixedYear, fixedMonth, 1);
            //var endLocal = new DateTime(localYear, localMonth, 1);

            var startLocal = localDate.AddDays(-31);
            var endLocal = localDate.Date.AddDays(1);

            var startUtc = TimeZoneInfo.ConvertTimeToUtc(startLocal, localZone);
            var endUtc = TimeZoneInfo.ConvertTimeToUtc(endLocal, localZone);

            Console.WriteLine($"Start UTC: {startUtc}, End UTC: {endUtc}");
            try
            {
                var rawReadings = await _context.SensorReadings
                    .Where(reading => reading.SensorId == 2 && reading.Date >= startUtc && reading.Date < endUtc)
                    .ToListAsync();
                   
                var readings = rawReadings
                    .GroupBy(reading =>
                    {
                        var localDate = TimeZoneInfo.ConvertTimeFromUtc(reading.Date, localZone);
                        return localDate.Date;
                    })
                    .Select(group => new
                    {
                        Date = group.Key,
                        // Average soil moisture for the day
                        // Should be rounded to 2 decimal places in React !!!
                        Soil = group.Average(reading => (reading.Value))
                    })
                    .OrderBy(reading => reading.Date)
                    .ToList();

                return Ok(readings);
            }
            catch (Exception ex)
            {
                return StatusCode(500, $"Error: {ex.Message}");
            }
        }

        //Test Controller if it takes data from correct days!!!
        [HttpGet("TempHumDataToday")]
        public async Task<IActionResult> GetTempHumDataToday()
        {
            var localZone = TimeZoneInfo.FindSystemTimeZoneById("Central European Standard Time");
            var localTimeNow = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, localZone);
            var todayLocal = localTimeNow.Date;
            var tommorowLocal = todayLocal.AddDays(1);

            var startUtc = TimeZoneInfo.ConvertTimeToUtc(todayLocal, localZone);
            var endUtc = TimeZoneInfo.ConvertTimeToUtc(tommorowLocal, localZone);

            try
            {
                var rawReadings = await _context.SensorReadings.Where(reading => (reading.SensorId == 0 || reading.SensorId == 1))
                    .Where(reading => reading.Date >= startUtc && reading.Date < endUtc)
                    .ToListAsync();
                
                var readings = rawReadings
                    .GroupBy(reading =>
                    {
                        var LocalTime = TimeZoneInfo.ConvertTimeFromUtc(reading.Date, localZone);
                        return new DateTime(LocalTime.Year, LocalTime.Month, LocalTime.Day, LocalTime.Hour, 0, 0);
                    })
                    .Select(group => new
                    {
                        Date = group.Key,
                        // Should be rounded to 2 decimal places in React !!!
                        Temperature = group.Where(reading => reading.SensorId == 0).Average(reading => reading.Value),
                        Humidity = group.Where(reading => reading.SensorId == 1).Average(reading => reading.Value)
                    })
                    .OrderBy(reading => reading.Date)
                    .ToList();

                return Ok(readings);
            }
            catch (Exception ex)
            {
                return StatusCode(500, $"Error: {ex.Message}");
            }
        }

        [HttpGet("TempHumDataWeek")]
        public async Task<IActionResult> GetTempHumData()
        {
            var localZone = TimeZoneInfo.FindSystemTimeZoneById("Central European Standard Time");
            var localTime = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, localZone);
          
            var endLocal = localTime.Date;
            var startLocal = localTime.AddDays(-6);

            var startUtc = TimeZoneInfo.ConvertTimeToUtc(startLocal, localZone);
            var endUtc = TimeZoneInfo.ConvertTimeToUtc(endLocal, localZone);
            
            try
            {
                var rawReadings = await _context.SensorReadings.Where(reading => (reading.SensorId == 0 || reading.SensorId == 1))
                    .Where(reading => reading.Date >= startUtc && reading.Date <= endUtc.AddDays(1))
                    .ToListAsync();

                var readings = rawReadings
                    .GroupBy(reading =>
                    {
                        var localDate= TimeZoneInfo.ConvertTimeFromUtc(reading.Date, localZone);
                        return localDate.Date;
                    })
                    .Select(group => new
                    {
                        Date = group.Key,
                        Temperature = group.Where(reading => reading.SensorId == 0).Average(reading => reading.Value),
                        Humidity = group.Where(reading => reading.SensorId == 1).Average(reading => reading.Value)
                    })
                    .OrderBy(reading => reading.Date)
                    .ToList();

               return Ok(readings); 
            }
            catch (Exception ex)
            {
                return StatusCode(500, $"Error: {ex.Message}");
            }
        }

        [HttpGet("TempHumDataLastMonth")]
        public async Task<IActionResult> GetTempHumDataLastMonth()
        {
            var localZone = TimeZoneInfo.FindSystemTimeZoneById("Central European Standard Time");
            var localDate = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, localZone);
            //var localMonth = localDate.Month;
            //var localYear = localDate.Year;

            //var fixedMonth = (localMonth == 1) ? 12 : localMonth - 1;
            //var fixedYear = (localMonth == 1) ? localYear - 1 : localYear;

            //var startLocal = new DateTime(fixedYear, fixedMonth, 1);
            //var endLocal = new DateTime(localYear, localMonth, 1);
            var startLocal = localDate.AddDays(-31);
            var endLocal = localDate.Date.AddDays(1);

            var startUtc = TimeZoneInfo.ConvertTimeToUtc(startLocal, localZone);
            var endUtc = TimeZoneInfo.ConvertTimeToUtc(endLocal, localZone);

            try
            {
                var rawReadings = await _context.SensorReadings
                    .Where(reading => (reading.SensorId == 0 || reading.SensorId == 1) && reading.Date >= startUtc && reading.Date < endUtc)
                    .ToListAsync();

                var readings = rawReadings
                    .GroupBy(reading =>
                    {
                        var localDate = TimeZoneInfo.ConvertTimeFromUtc(reading.Date, localZone);
                        return localDate.Date;
                    })
                    .Select(group => new
                    {
                        Date = group.Key,
                        // Average soil moisture for the day
                        // Should be rounded to 2 decimal places in React !!!
                        Temperature = group.Where(reading => reading.SensorId == 0).Average(reading => (reading.Value)),
                        Humidity = group.Where(reading => reading.SensorId == 1).Average(reading => (reading.Value))
                    })
                    .OrderBy(reading => reading.Date)
                    .ToList();

                return Ok(readings);
            }
            catch (Exception ex)
            {
                return StatusCode(500, $"Error: {ex.Message}");
            }
        }
    }
}