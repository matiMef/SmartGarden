// TempHumLineChart.tsx
import React from "react";
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend,
} from "chart.js";
import { Line } from "react-chartjs-2";

// Rejestrujemy potrzebne modu³y Chart.js
ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

// Typ danych przekazywanych do komponentu
interface TempHumLineChartProps {
    temperatureData: Array<number>;
    humidityData: Array<number>;
    dates?: Array<string>;
}

// Nasz komponent
const TempHumLineChart: React.FC<TempHumLineChartProps> = ({
    temperatureData,
    humidityData,
    dates
}) => {
    const labels = dates; // przyk³adowe dni

    const data = {
        labels,
        datasets: [
            {
                label: "Temperature (C)",
                data: temperatureData,
                borderColor: "rgb(255, 99, 132)",
                backgroundColor: "rgba(255, 99, 132, 0.5)",
                yAxisID: "y",
            },
            {
                label: "Humidity (%)",
                data: humidityData,
                borderColor: "rgb(54, 162, 235)",
                backgroundColor: "rgba(54, 162, 235, 0.5)",
                yAxisID: "y1",
            },
        ],
    };

    const options = {
        responsive: true,
        interaction: {
            mode: "index" as const,
            intersect: false,
        },
        stacked: false,
        plugins: {
            legend: {
                labels: {
                    color: "#F8F8F9",
                    size: 14,
                    weight: "bold", 
                },
            },
        },
        legend: {
            labels: {
                color: "#F8F8F9", // kolor etykiet legendy
            },
        },
        scales: {
            y: {
                type: "linear" as const,
                display: true,
                position: "left",
                grid: {
                    color: "rgba(255,255,255,0.4)", // kolor linii siatki
                },
                ticks: {
                    color: "#F8F8F9", 
                },
            },
            y1: {
                type: "linear" as const,
                display: true,
                position: "right",
                grid: {
                    drawOnChartArea: false,
                    color: "rgba(255,255,255,0.4)",
                },
                ticks: {
                    color: "#F8F8F9",
                },  
            },
            x: {
                grid: {
                    color: "rgba(255,255,255,0.4)", // kolor linii siatki
                },
                ticks: {
                    color: "#F8F8F9",
                },
            },
        },
    } as any;

    return <Line options={options} data={data} />;
};

export default TempHumLineChart;
