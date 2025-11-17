namespace Garden.Api.Models
{
	public class SensorReadings
	{
		public int Id { get; set; } 
		public float SensorId { get; set; }
		public DateTime Date { get; set; } = DateTime.UtcNow;
		public float Value{ get; set; }
	}
}
