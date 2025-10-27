namespace Garden.Api.Models
{
	public class SensorReadings
	{
		public int Id { get; set; } // klucz g³ówny
		public float SensorId { get; set; }
		public DateTime Date { get; set; } = DateTime.UtcNow;
		public float Value{ get; set; }
	}
}
