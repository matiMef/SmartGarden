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

interface SoilGaugeProps {
    soil: number; // wartoœæ, któr¹ dostajesz z API
}

// Pomocnicze kolory
const COLORS = ['rgb(140, 214, 16)', 'rgb(239, 198, 0)', 'rgb(231, 24, 49)'];

function colorForValue(v: number) {
    if (v < 30) return COLORS[2];
    if (v < 50) return COLORS[1];
    return COLORS[0];
}

const centerTextSoil: Plugin<'doughnut'> = {
    id: 'centerTextSoil',
    afterDraw(chart) {
        const { ctx, chartArea } = chart;
        const soil = (chart.config.options as any).soil as number;

        ctx.save();
        ctx.font = 'bold 30px Arial';
        ctx.fillStyle = colorForValue(Math.floor(soil));
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(`${soil}%`, chartArea.width / 2 + chartArea.left, chartArea.height / 1.4);
    },
};


const SoilGauge: React.FC<SoilGaugeProps> = ({ soil }) => {
    const [displaySoil, setDisplaySoil] = useState(soil);

    useEffect(() => {
        const step = (soil - displaySoil) / 10;
        const interval = setInterval(() => {
            setDisplaySoil(prev => {
                const next = prev + step;
                if (Math.abs(next - soil) < 0.5) {
                    clearInterval(interval);
                    return soil;
                }
                return next;
            });
        }, 50);
        return () => clearInterval(interval);
    }, [soil, displaySoil]);

    const data: ChartData<'doughnut'> = {
        datasets: [
            {
                data: [soil, 100 - soil],
                backgroundColor: [colorForValue(Math.floor(soil)), '#eaeaea'],
                borderWidth: 0,
            },
        ],
    };

    const options: ChartOptions<'doughnut'> = {
        soil,
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
                plugins={[centerTextSoil]} />
        </div>

    );
};

export default SoilGauge;

