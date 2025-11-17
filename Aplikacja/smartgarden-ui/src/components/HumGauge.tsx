import React, { useState, useEffect } from 'react';
import {
    Chart as ChartJS,
    ArcElement,
    Tooltip,
    ChartOptions,
    ChartData,
    Plugin
} from 'chart.js';
import { Doughnut} from 'react-chartjs-2';

ChartJS.register(ArcElement, Tooltip);

interface HumidityGaugeProps {
    humidity: number;
}

const COLORS = ['rgb(140, 214, 16)', 'rgb(239, 198, 0)', 'rgb(231, 24, 49)'];

function colorForValue(v: number) {
    if (v < 60) return COLORS[0];
    if (v < 75) return COLORS[1];
    return COLORS[2];
}

const centerTextHumidity: Plugin<'doughnut'> = {
    id: 'centerTextHumidity',
    afterDraw(chart) {
        const { ctx, chartArea } = chart;
        const humidity = (chart.config.options as any).humidity as number;

        ctx.save();
        ctx.font = 'bold 30px Arial';
        ctx.fillStyle = colorForValue(Math.floor(humidity));
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(`${humidity}%`, chartArea.width / 2 + chartArea.left, chartArea.height / 1.4);

    },
};


const HumidityGauge: React.FC<HumidityGaugeProps> = ({ humidity }) => {
    const [displayHumidity, setDisplayHumidity] = useState(humidity);

    useEffect(() => {
        const step = (humidity - displayHumidity) / 10;
        const interval = setInterval(() => {
            setDisplayHumidity(prev => {
                const next = prev + step;
                if (Math.abs(next - humidity) < 0.5) {
                    clearInterval(interval);
                    return humidity;
                }
                return next;
            });
        }, 50);
        return () => clearInterval(interval);
    }, [humidity]);

    const data: ChartData<'doughnut'> = {
        datasets: [
            {
                data: [humidity, 100 - humidity],
                backgroundColor: [colorForValue(Math.floor(humidity)), '#eaeaea'],
                borderWidth: 0,
            },
        ],
    };

    const options: ChartOptions<'doughnut'> = {
        humidity,
        aspectRatio: 2,
        rotation: -90,
        circumference: 180,
        cutout: '70%',
        devicePixelRatio: window.devicePixelRatio,
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
                plugins={[centerTextHumidity]} />
        </div>

    );
};

export default HumidityGauge;
