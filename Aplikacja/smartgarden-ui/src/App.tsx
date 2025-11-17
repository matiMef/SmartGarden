import React, { useEffect, useState } from "react";
import HumidityGauge from "./components/HumGauge";
import SoilGauge from "./components/SoilGauge";
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

    // This function fetches data from the Arduino API
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
            if (fixedData.profile === 0) { fixedData.profile = "Default" } else if (fixedData.profile === 1) { fixedData.profile = "Low Soil" } else (fixedData.profile = "High Soil");
            if (fixedData.distance < 0) { fixedData.distance = 0 };
            setData(fixedData);
        }
        catch (err) {
            console.error("Error while getting data:", err);
        }
    };

    // This function fetches soil data for today
    const fetchSoilDataToday = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/SoilDataToday");
            const json = await res.json();

            const fixedSoilData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleTimeString("pl-PL", { hour: "2-digit", minute: "2-digit" }),
                soil: Math.floor((item.soil / 255) * 100)
            }));

            const soils = fixedSoilData.map((it: any) => it.soil);
            const dates = fixedSoilData.map((it: any) => it.date);

            //console.log("Soil data today (objects):", fixedSoilData);
            //console.log("Soil values array:", soils);
            //console.log("Soil dates array:", dates);

            setSoilValues(soils);
            setSoilDates(dates);
        } catch (err) {
            console.error("Error while fetching soil data for today:", err);
        }
    };

    // This function fetches soil data for Last 7 active days
    const fetchSoilDataWeek = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/SoilDataWeek");
            const json = await res.json();
            const fixedSoilData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleDateString("pl-PL", { day: "2-digit", month: "2-digit" }),
                soil: Math.floor((item.soil / 255) * 100)
            }));

            const soils = fixedSoilData.map((it: any) => it.soil);
            const dates = fixedSoilData.map((it: any) => it.date);

            //console.log("Soil values array:", soils);
            //console.log("Soil dates array:", dates);

            setSoilValues(soils);
            setSoilDates(dates);
        }
        catch (err) {
            console.error("Error while fetching soil data for the week:", err);
        }
    };

    // This function fetches soil data for last 30 active days
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

            //console.log("Soil data today (objects):", fixedSoilData);
            //console.log("Soil values array:", soils);
            //console.log("Soil dates array:", dates);

            setSoilValues(soils);
            setSoilDates(dates);
        }

        catch (err) {
            console.error("Error while fetching soil data for the month:", err);
        }
    };

    // This function fetches temperature and humidity data for today
    const fetchTempHumDataToday = async () => {
        try {
            const res = await fetch("http://localhost:5246/api/Query/TempHumDataToday");
            const json = await res.json();

            const fixedTempHumData = json.map((item: any) => ({
                date: new Date(item.date).toLocaleTimeString("pl-PL", { hour: "2-digit", minute: "2-digit" }),
                temperature: Math.floor((item.temperature / 100) * 100),
                humidity: Math.floor((item.humidity / 100) * 100)
            }));

            const temps = fixedTempHumData.map((it: any) => it.temperature);
            const hums = fixedTempHumData.map((it: any) => it.humidity);
            const dates = fixedTempHumData.map((it: any) => it.date);

            //console.log("TempHum data today (objects):", fixedTempHumData);
            //console.log("Temperature values array:", temps);
            //console.log("Humidity values array:", hums);
            //console.log("Dates array:", dates);

            setTempValues(temps);
            setHumValues(hums);
            setTempHumDates(dates);
        } catch (err) {
            console.error("Error while fetching temp/hum data for today:", err);
        }
    };


    // This function fetches temperature and humidity data for last 7 active days
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

    // This function fetches temperature and humidity data for last 30 active days
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

    // This function sends a command to start watering
    const sendCommand = async () => {
        try {
            const response = await fetch("http://localhost:5246/api/Arduino/sendCommand",
                {
                    method: "POST",
                    headers:
                    {
                        "Content-Type": "application/json",
                    },
                    body: JSON.stringify({ payload: command }),
                });
            console.log("Command sent:", command);

            const text = await response.text();
            if (text.includes("LOW_WATER_LEVEL")) {
                alert("🚱 Low water level — watering aborted!");
            }
            if (text.includes("WATERING_ACTIVE")) {
                alert("⚠️ Watering active — watering aborted!");
            }
            if (text.includes("WATERING_STARTED")) {
                alert("💧 Watering has started!");
            }
        }
        catch (err) {
            console.error("Error while sending command:", err);
        }
    };

    useEffect(() => {
        // Initial fetch and setting up interval for periodic fetching
        fetchDataFromArduino();
        const interval = setInterval(fetchDataFromArduino, 60000);
        // Cleanup interval on component unmount
        return () => clearInterval(interval);
    }, []);

    useEffect(() => {
        // Fetch data based on selected periods

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
        <div className="App"> ]
            {data ? (
                <div>
                    <div className="header">
                        <div className="header-text">🌱 Smart Garden Dashboard</div>
                    </div>
                    <div className="margin"></div>
                    <div className="main-content-wrapper">
                    <div className="containers-wrapper">
                        <div className="summary-container">
                            <h5> 📊 Summary</h5>
                            <div className="quick-info">
                                <p>🌡️ Temperature: {data.temperature} °C</p>
                                <p>💧 Humidity: {data.humidity} %</p>
                                <p>🌿 Soil moisture: {data.soil} %</p>
                                <p>💡 Illuminance level: {data.lux} lux</p>
                                <p>🌊 Water Level: {data.distance} %</p>
                                <p>🌵 Profile: {data.profile}</p>
                            </div>
                            <div className="quick-watering">
                                <div className="watering-item">
                                    ️🚿 Quick Watering:

                                    <Select size="small" value={command} sx={{
                                        color: "white",
                                        height: "32px",
                                        '.MuiSelect-icon': { color: 'white' },
                                        '.MuiOutlinedInput-notchedOutline': { borderColor: 'white' },
                                        '&:hover .MuiOutlinedInput-notchedOutline': { borderColor: '#ccc' },
                                        '&.Mui-focused .MuiOutlinedInput-notchedOutline': { borderColor: 'white' },
                                    }} onChange={(e) => setCommand(e.target.value)}>
                                        <MenuItem value="start_short">Short</MenuItem>
                                        <MenuItem value="start_mid">Mid</MenuItem>
                                        <MenuItem value="start_long">Long</MenuItem>
                                    </Select>

                                    <Button size="small" variant="contained" sx={{ font: 'Arial', fontWeight: '700' }} onClick={sendCommand}>Start</Button>
                                </div>
                            </div>
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

                    <div className="charts-container">
                        <div className="chart-container">
                            <div className="flex gap-2 mb-4">
                                <Select size="small" value={periodTempHum} sx={{
                                    color: "#2196f3", // text inside select                   
                                    '.MuiSelect-icon': { color: '#2196f3' },  // arrow color
                                    '.MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' },  // border color
                                    '&:hover .MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' }, // border color on hover
                                    '&.Mui-focused .MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' }, // border color when focused
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
                                    color: "#f50057",
                                    '.MuiSelect-icon': { color: '#f50057' },
                                    '.MuiOutlinedInput-notchedOutline': { borderColor: '#f50057' },
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
                </div>
            ) : (
                <div>
                    <div className="header">
                        <div className="header-text"><div className="header-item1">🌱 Smart Garden Dashboard</div><div className="header-item2">❌ No Connection</div></div>
                    </div>
                        <div className="margin"></div>
                        <div className="main-content-wrapper">
                    <div className="containers-wrapper">
                        <div className="summary-container">
                            <h5> 📊 Summary</h5>
                            <div className="quick-info">
                                <p>🌡️ Temperature: - °C</p>
                                <p>💧 Humidity: - %</p>
                                <p>🌿 Soil moisture: - %</p>
                                <p>💡 Illuminance level: - lux</p>
                                <p>🌊 Water Level: - %</p>
                                <p>🌵 Profile: - </p>
                            </div>
                            <div className="quick-watering">
                                <div className="watering-item">
                                    ️🚿 Quick Watering:

                                    <Select size="small" value={command} disabled sx={{
                                        color: "white",
                                        height: "32px",
                                        '.MuiSelect-icon': { color: 'white' },
                                        '.MuiOutlinedInput-notchedOutline': { borderColor: 'white' },
                                        '&:hover .MuiOutlinedInput-notchedOutline': { borderColor: '#ccc' },
                                        '&.Mui-focused .MuiOutlinedInput-notchedOutline': { borderColor: 'white' },
                                    }} onChange={(e) => setCommand(e.target.value)}>
                                        <MenuItem value="start_short">Short</MenuItem>
                                        <MenuItem value="start_mid">Mid</MenuItem>
                                        <MenuItem value="start_long">Long</MenuItem>
                                    </Select>

                                    <Button size="small" variant="contained" disabled sx={{ font: 'Arial', fontWeight: '700' }} onClick={sendCommand}>Start</Button>
                                </div>
                            </div>


                        </div>

                        <div className="gauge-container">
                            <h5> 📈 Humidity</h5>
                            <HumidityGauge
                                humidity={0}
                            />
                        </div>
                        <div className="gauge-container">
                            <h5> 📈 Soil</h5>

                            <SoilGauge
                                soil={0}
                            />
                        </div>
                        <div className="gauge-container">
                            <h5> 📈 Water level</h5>
                            <WaterLevelGauge
                                waterLevel={0}
                            />
                        </div>
                    </div>

                    <div className="charts-container">
                        <div className="chart-container"  >
                            <div className="flex gap-2 mb-4">
                                <Select size="small" value={periodTempHum} sx={{
                                    color: "#2196f3", // text inside select                   
                                    '.MuiSelect-icon': { color: '#2196f3' },  // arrow color
                                    '.MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' },  // border color
                                    '&:hover .MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' }, // border color on hover
                                    '&.Mui-focused .MuiOutlinedInput-notchedOutline': { borderColor: '#2196f3' }, // border color when focused
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

                        <div className="chart-container">
                            <div className="flex gap-2 mb-4">
                                <Select size="small" value={periodSoil} sx={{
                                    color: "#f50057",
                                    '.MuiSelect-icon': { color: '#f50057' },
                                    '.MuiOutlinedInput-notchedOutline': { borderColor: '#f50057' },
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
                </div>
            )}

        </div>
    );
}

export default App;
