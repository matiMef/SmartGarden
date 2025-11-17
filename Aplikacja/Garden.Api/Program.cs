using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Options;
using Garden.Api.Data;
using Microsoft.EntityFrameworkCore;
using Garden.Api.Services;

var builder = WebApplication.CreateBuilder(args);

var ConnectionString = builder.Configuration.GetConnectionString("DefaultConnection");

builder.Services.AddControllers(); 
builder.Services.AddHttpClient();

builder.Services.AddDbContext<AppDbContext>(options =>
    options.UseNpgsql(ConnectionString));

builder.Services.AddCors(options =>
{
    options.AddPolicy("AllowReactApp", policy =>
    {
        policy.WithOrigins("http://localhost:3000") 
              .AllowAnyHeader()
              .AllowAnyMethod();
    });

});

builder.Services.AddHostedService<DataService>();
builder.Services.AddHttpClient<DataService>();

var app = builder.Build();

app.UseCors("AllowReactApp");
app.MapControllers();

app.Run();
