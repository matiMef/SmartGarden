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

ChartJS.register(ArcElement, Tooltip);

interface WaterLevelGaugeProps {
    waterLevel: number; // wartoœæ, któr¹ dostajesz z API
}

// Pomocnicze kolory
const COLORS = ['rgb(231, 24, 49)', 'rgb(239, 198, 0)', 'rgb(140, 214, 16)'];

function colorForValue(v: number) {
    if (v < 25) return COLORS[0];
    if (v < 75) return COLORS[1];
    return COLORS[2];
}

const centerTextWaterLevel: Plugin<'doughnut'> = {
    id: 'centerTextWaterLevel',
    afterDraw(chart) {
        const { ctx, chartArea } = chart;
        const waterLevel = (chart.config.options as any).waterLevel as number;

        ctx.save();
        ctx.font = 'bold 30px Arial';
        ctx.fillStyle = colorForValue(Math.floor(waterLevel));
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(`${waterLevel}%`, chartArea.width / 2 + chartArea.left, chartArea.height / 1.4);

    },
};


const WaterLevelGauge: React.FC<WaterLevelGaugeProps> = ({ waterLevel }) => {
    const [displayWaterLevel, setDisplayWaterLevel] = useState(waterLevel);

    useEffect(() => {
        const step = (waterLevel - displayWaterLevel) / 10;
        const interval = setInterval(() => {
            setDisplayWaterLevel(prev => {
                const next = prev + step;
                if (Math.abs(next - waterLevel) < 0.5) {
                    clearInterval(interval);
                    return waterLevel;
                }
                return next;
            });
        }, 50);
        return () => clearInterval(interval);
    }, [waterLevel]);

    const data: ChartData<'doughnut'> = {
        datasets: [
            {
                data: [waterLevel, 100 - waterLevel],
                backgroundColor: [colorForValue(Math.floor(waterLevel)), '#eaeaea'],
                borderWidth: 0,
            },
        ],
    };

    const options: ChartOptions<'doughnut'> = {
        waterLevel,
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
                plugins={[centerTextWaterLevel]} />
        </div>

    );
};

export default WaterLevelGauge;
