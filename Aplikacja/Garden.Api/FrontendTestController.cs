using Microsoft.AspNetCore.Mvc;

namespace SmartGarden.Api.Controllers
{
	[ApiController]
	[Route("api/[controller]")]
	public class TestFrontendController : ControllerBase
	{
		[HttpGet("status")]
		public IActionResult GetStatus()
		{
			// Sending hardcoded data for forntend testing
			return Ok(new
			{
				temperature = 25.4,
				humidity = 60.2,
				soil = 120,
				profile = "Default"
			});
		}
    }


}