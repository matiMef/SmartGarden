import React, { useEffect, useState } from "react";
import TemperatureGauge from "./components/gauge";
import HumidityGauge from "./components/HumGauge";
import SoilGauge from "./components/soilGauge";
import WaterLevelGauge from "./components/WaterGauge";
import TempHumLineChart from "./components/TempHumLineChart";
import SoilLineChart from "./components/SoilLineChart";
import { Button, Select } from "@mui/material";
import MenuItem from '@mui/material/MenuItem';



import "./App.css";
function App() {
    const [data, setData] = useState<any>(null);
    const [soilValues, setSoilValues] = useState<number[]>([]);
    const [soilDates, setSoilDates] = useState<string[]>([]);
    const [tempValues, setTempValues] = useState<number[]>([]);
    const [tempHumDates, setTempHumDates] = useState<string[]>([]);
    const [humValues, setHumValues] = useState<number[]>([]);
    const [command, setCommand] = useState<string>("start_short");
    const [periodTempHum, setPeriodTempHum] = useState<"day" | "week" | "month">("day");
    const [periodSoil, setPeriodSoil] = useState<"day" | "week" | "month">("day");

    // Funkcja pobierająca dane
    const fetchDataFromArduino = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Arduino/getData");
            const json = await res.json();
            const fixedData =
            {
                ...json,
                soil: Math.floor((json.soil as number / 255) * 100), // przeliczenie na procent
                distance: Math.floor(((13.26 - json.distance) * 100) / 13.26)
            };
            setData(fixedData);
        }
        catch (err) {
            console.error("Error while getting data:", err);
        }
    };

    const fetchSoilDataToday = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/SoilDataToday");
            const json = await res.json();

            const fixedSoilData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleTimeString("pl-PL", { hour: "2-digit", minute: "2-digit" }),
                soil: Math.floor((item.soil / 255) * 100)
            }));

            // Dwie oddzielne tablice: wartości soil i daty
            const soils = fixedSoilData.map((it: any) => it.soil);
            const dates = fixedSoilData.map((it: any) => it.date);

            console.log("Soil data today (objects):", fixedSoilData);
            console.log("Soil values array:", soils);
            console.log("Soil dates array:", dates);

            setSoilValues(soils);
            setSoilDates(dates);
        } catch (err) {
            console.error("Error while fetching soil data for today:", err);
        }
    };

    const fetchSoilDataWeek = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/SoilDataWeek");
            const json = await res.json();
            const fixedSoilData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleDateString("pl-PL", { day: "2-digit", month: "2-digit" }),
                soil: Math.floor((item.soil / 255) * 100)
            }));

            // Dwie oddzielne tablice: wartości soil i daty
            const soils = fixedSoilData.map((it: any) => it.soil);
            const dates = fixedSoilData.map((it: any) => it.date);

            console.log("Soil values array:", soils);
            console.log("Soil dates array:", dates);

            setSoilValues(soils);
            setSoilDates(dates);
        }
        catch (err) {
            console.error("Error while fetching soil data for the week:", err);
        }
    };

    const fetchSoilDataLastMonth = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/SoilDataLastMonth");
            const json = await res.json();

            const fixedSoilData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleDateString("pl-PL", { day: "2-digit", month: "2-digit" }),
                soil: Math.floor((item.soil / 255) * 100)
            }));

            const soils = fixedSoilData.map((it: any) => it.soil);
            const dates = fixedSoilData.map((it: any) => it.date);

            console.log("Soil data today (objects):", fixedSoilData);
            console.log("Soil values array:", soils);
            console.log("Soil dates array:", dates);

            setSoilValues(soils);
            setSoilDates(dates);
        }

        catch (err) {
            console.error("Error while fetching soil data for the month:", err);
        }
    };

    const fetchTempHumDataToday = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/TempHumDataToday");
            const json = await res.json();

            //if (json !== null) {
            const fixedTempHumData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleTimeString("pl-PL", { hour: "2-digit", minute: "2-digit" }),
                temperature: Math.floor((item.temperature / 100) * 100),
                humidity: Math.floor((item.humidity / 100) * 100)
            }));


            // Dwie oddzielne tablice: wartości soil i daty
            const temps = fixedTempHumData.map((it: any) => it.temperature);
            const hums = fixedTempHumData.map((it: any) => it.humidity);
            const dates = fixedTempHumData.map((it: any) => it.date);

            console.log("TempHum data today (objects):", fixedTempHumData);
            console.log("Temperature values array:", temps);
            console.log("Humidity values array:", hums);
            console.log("Dates array:", dates);

            setTempValues(temps);
            setHumValues(hums);
            setTempHumDates(dates);
        } catch (err) {
            console.error("Error while fetching temp/hum data for today:", err);
        }
    };

    const fetchTempHumDataWeek = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/TempHumDataWeek");
            const json = await res.json();
            const fixedTempHumData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleDateString("pl-PL", { day: "2-digit", month: "2-digit" }),
                temperature: Math.floor((item.temperature / 100) * 100),
                humidity: Math.floor((item.humidity / 100) * 100)
            }));

            const temps = fixedTempHumData.map((item: any) => item.temperature);
            const hums = fixedTempHumData.map((item: any) => item.humidity);
            const dates = fixedTempHumData.map((item: any) => item.date);

            setTempValues(temps);
            setHumValues(hums);
            setTempHumDates(dates);
        }
        catch (err) {
            console.error("Error while fetching soil data for the week:", err);
        }
    };
    const fetchTempHumDataLastMonth = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/TempHumDataLastMonth");
            const json = await res.json();
            const fixedTempHumData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleDateString("pl-PL", { day: "2-digit", month: "2-digit" }),
                temperature: Math.floor((item.temperature / 100) * 100),
                humidity: Math.floor((item.humidity / 100) * 100)
            }));

            const temps = fixedTempHumData.map((item: any) => item.temperature);
            const hums = fixedTempHumData.map((item: any) => item.humidity);
            const dates = fixedTempHumData.map((item: any) => item.date);

            setTempValues(temps);
            setHumValues(hums);
            setTempHumDates(dates);
        }
        catch (err) {
            console.error("Error while fetching soil data for the month:", err);
        }
    };

    const sendCommand = async () => {
        try {
            await fetch("http://localhost:5246/api/Arduino/sendCommand",
                {
                    method: "POST",
                    headers:
                    {
                        "Content-Type": "application/json",
                    },
                    body: JSON.stringify({ payload: command }),
                });
            console.log("Command sent:", command);
        }
        catch (err) {
            console.error("Error while sending command:", err);
        }
    };

    useEffect(() => {
        // Pierwsze pobranie danych
        fetchDataFromArduino();

        // Ustawienie odświeżania co 10 sekund
        const interval = setInterval(fetchDataFromArduino, 10000);

        // Sprzątanie po odmontowaniu komponentu
        return () => clearInterval(interval);
    }, []);

    useEffect(() => {
        if (periodTempHum === "day") {
            fetchTempHumDataToday();
        } else if (periodTempHum === "week") {
            fetchTempHumDataWeek();
        } else if (periodTempHum === "month") {
            fetchTempHumDataLastMonth();
        };

        if (periodSoil === "day") {
            fetchSoilDataToday();
        } else if (periodSoil === "week") {
            fetchSoilDataWeek();
        } else if (periodSoil === "month") {
            fetchSoilDataLastMonth();
        };

        //console.log("Soil data today fetched:", soilData);
    }, [periodTempHum, periodSoil]);


    return (

        <div className="App">
            <div className="header">
                <div className="header-text"> 🌱 Smart Garden Dashboard</div>
            </div>
            {data ? (
                <div>
                    <div className="quick-info-wrapper">
                        <div className="quick-info">
                            🌡️ Temperature: {data.temperature} °C
                            💧 Humidity: {data.humidity} %
                            🌿 Soil moisture: {data.soil} %
                            💡 Illuminance level: {data.lux} lux
                            🌊 Water Level: {data.distance} %
                            <div className="quick-watering">
                                ️🚿 Quick Watering:

                                <Select size="small" value={command} sx={{
                                    color: "white",                   // tekst wewnątrz selecta
                                    '.MuiSelect-icon': { color: 'white' }, // kolor strzałki
                                    '.MuiOutlinedInput-notchedOutline': { borderColor: 'white' }, // ramka
                                    '&:hover .MuiOutlinedInput-notchedOutline': { borderColor: '#ccc' },
                                    '&.Mui-focused .MuiOutlinedInput-notchedOutline': { borderColor: 'white' },
                                }} onChange={(e) => setCommand(e.target.value)}>
                                    <MenuItem value="start_short">Short</MenuItem>
                                    <MenuItem value="start_mid">Mid</MenuItem>
                                    <MenuItem value="start_long">Long</MenuItem>
                                </Select>

                                <Button size="small" variant="contained" sx={{font:'Arial', fontWeight: '700'}} onClick={sendCommand}>Start</Button>
                            </div>
                        </div>

                    </div>
                    <div className="gauges-wrapper">
                        <div className="gauge-container">
                            <h5> 📈 Temperature</h5>
                            <TemperatureGauge
                                temperature={data.temperature}
                            />
                        </div>

                        <div className="gauge-container">
                            <h5> 📈 Humidity</h5>
                            <HumidityGauge
                                humidity={data.humidity}
                            />
                        </div>
                        <div className="gauge-container">
                            <h5> 📈 Soil</h5>

                            <SoilGauge
                                soil={data.soil}
                            />
                        </div>
                        <div className="gauge-container">
                            <h5> 📈 Water level</h5>
                            <WaterLevelGauge
                                waterLevel={data.distance}
                            />
                        </div>
                    </div>

                    <div className="charts-container" style={{ height: 500, width: 1800 }}>
                        <div className="chart-container" style={{ height: 400, width: 700 }} >
                            <div className="flex gap-2 mb-4">
                                <Select size="small" value={periodTempHum} sx={{
                                    color: "#2196f3",                   // tekst wewnątrz selecta
                                    '.MuiSelect-icon': { color: '#2196f3' }, // kolor strzałki
                                    '.MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' }, // ramka
                                    '&:hover .MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' },
                                    '&.Mui-focused .MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' },
                                }} onChange={(e) => setPeriodTempHum(e.target.value)}>
                                    <MenuItem value="day">Day</MenuItem>
                                    <MenuItem value="week">Week</MenuItem>
                                    <MenuItem value="month">Month</MenuItem>
                                </Select>
                            </div>


                            <TempHumLineChart
                                temperatureData={tempValues}
                                humidityData={humValues}
                                dates={tempHumDates}
                            />
                        </div>

                        <div className="chart-container" style={{ height: 400, width: 700 }} >
                            <div className="flex gap-2 mb-4">
                                <Select size="small" value={periodSoil} sx={{
                                    color: "#f50057",// tekst wewnątrz selecta
                                    
                                    '.MuiSelect-icon': { color: '#f50057' }, // kolor strzałki
                                    '.MuiOutlinedInput-notchedOutline': { borderColor: '#f50057' }, // ramka
                                    '&:hover .MuiOutlinedInput-notchedOutline': { borderColor: '#f50057' },
                                    '&.Mui-focused .MuiOutlinedInput-notchedOutline': { borderColor: '#f50057' },
                                }} onChange={(e) => setPeriodSoil(e.target.value)}>
                                    <MenuItem value="day">Day</MenuItem>
                                    <MenuItem value="week">Week</MenuItem>
                                    <MenuItem value="month">Month</MenuItem>
                                </Select>
                            </div>
                            <SoilLineChart
                                soilData={soilValues} soilDates={soilDates}
                            />
                        </div>
                    </div>
                </div>
            ) : (
                <p>Loading data...</p>
            )}

        </div>
    );
}

export default App;
