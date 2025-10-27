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

// Rejestrujemy potrzebne moduły Chart.js
ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

 //Typ danych przekazywanych do komponentu
interface SoilLineChartProps {
    soilData: Array<number>;
    soilDates: Array<string>;
}

// Nasz komponent
const SoilLineChart: React.FC<SoilLineChartProps> = ({
    soilData,
    soilDates
}) => {
    const labels = soilDates; // przykładowe dni

    const data = {
        labels,
        datasets: [
            {
                label: "Soil moisture level (%)",
                data: soilData,
                borderColor: "rgb(255, 99, 132)",
                backgroundColor: "rgba(255, 99, 132, 0.5)",
                yAxisID: "y",

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
        scales: {
            y: {
                type: "linear" as const,
                display: true,
                position: "left",
                min: 0,      // stały dolny zakres
                max: 100,    // stały górny zakres
                grid: {
                    color: "rgba(255, 255, 255, 0.4)",
                },
                ticks: {
                    color: "#F8F8F9",
                },
            },
            x: {
                grid: {
                    color: "rgba(255, 255, 255, 0.4)",
                },
                ticks: {
                    color: "#F8F8F9",
                },
            }
        },
    } as any;

    return <Line options={options} data={data} />;
};

export default SoilLineChart;
