using Garden.Api.Models;
using Microsoft.EntityFrameworkCore;

namespace Garden.Api.Data
{
    public class AppDbContext : DbContext
    {
        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

        public DbSet<SensorReadings> SensorReadings { get; set; }
        public DbSet<Sensors> Sensors{ get; set; }
    }
}
