import React, { useState, useEffect } from 'react';
import {
    Chart as ChartJS,
    ArcElement,
    Tooltip,
    ChartOptions,
    ChartData,
    Plugin
} from 'chart.js';
import { Doughnut } from 'react-chartjs-2';
import "../App.css";  


ChartJS.register(ArcElement, Tooltip);

interface TemperatureGaugeProps {
    temperature: number; // wartość, którą dostajesz z API
}

// Pomocnicze kolory
const COLORS = ['rgb(50, 100, 255)', 'rgb(5, 220, 250)', 'rgb(140, 214, 16)', 'rgb(239, 198, 0)', 'rgb(231, 24, 49)'];

function colorForValue(v: number) {
    if (v < 0) return COLORS[0];
    if (v < 15) return COLORS[1];
    if (v < 30) return COLORS[2];
    if (v < 40) return COLORS[3];
    return COLORS[4];
}

const centerTextTemperature: Plugin<'doughnut'> = {
    id: 'centerTemperature',
    afterDraw(chart) {
        const { ctx, chartArea } = chart;
        const temperature = (chart.config.options as any).temperature as number;

        ctx.save();
        ctx.font = 'bold 30px Arial';
        ctx.fillStyle = colorForValue(Math.floor(temperature));
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(`${temperature}°C`, chartArea.width / 2 + chartArea.left, chartArea.height / 1.4);

    },
};

const TemperatureGauge: React.FC<TemperatureGaugeProps> = ({ temperature }) => {
    const [displayHumidity, setDisplayHumidity] = useState(temperature);

    useEffect(() => {
        const step = (temperature - displayHumidity) / 10;
        const interval = setInterval(() => {
            setDisplayHumidity(prev => {
                const next = prev + step;
                if (Math.abs(next - temperature) < 0.5) {
                    clearInterval(interval);
                    return temperature;
                }
                return next;
            });
        }, 50);
        return () => clearInterval(interval);
    }, [temperature]);

    const data: ChartData<'doughnut'> = {
        datasets: [
            {
                data: [temperature > 0 ? temperature + 20 : 20 + temperature, temperature > 0 ? 70 - temperature : temperature + 70],
                backgroundColor: [colorForValue(Math.floor(temperature)), '#eaeaea'],
                borderWidth: 0,
            },
        ],
    };

    const options: ChartOptions<'doughnut'> = {
        temperature,
        aspectRatio: 2,
        rotation: -90,
        circumference: 180,
        cutout: '70%',
        animation: {
            animateRotate: false,
            animateScale: false,
            duration: 1000,
        },
        transitions: {
            active: {
                animation: {
                    duration: 1000,
                    easing: 'easeOutCubic',
                },
            },
        },
        plugins: {
            legend: { display: false },
            tooltip: { enabled: false },
        },
    } as any;

    return (
        <div className="gauge">
            <Doughnut
                data={data}
                options={options}
                plugins={[centerTextTemperature]} />
        </div>
    );
};

export default TemperatureGauge;
