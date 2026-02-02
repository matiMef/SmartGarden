## System nawadniania

System tworzą mikrokontroler **Arduino UNO R4 WiFi** wraz z czujnikami **AHT10**,**VEML7700**, **HC-SR04P**, **LM393** oraz mobilny moduł podlewania składający się z pomki głędbinowej, zbiornika i podejmnika z akumulatorami.\
Urządzenie jest obsługiwane przez aplikację webową SPA zaimplementowaną z wykrozystaniem technologii **REACT** oraz **ASP.NET**.

### Urządzenie podlewające

#### Najważniejsze funkcjonalności
1. Automatyczne podlewanie
2. Monitorowanie warunków środowiskowych
3. Udostępnianie AP
4. Zmiana danych logowania do AP
5. Zmiana profilu nawadniania
6. Obsługa błędów magistrali I2C
7. Wyświetalnie danych i obsługa wyświetlacza
8. Kontrolowanie poziomu wody w zbiorniku

#### Algorytm podlewania 

Algorytm dostosowywuje czas podlewania na podstawie aktualnych danych środowiskowych, które są uzyskiwane co 10 sekund z czujników. Algorytm priorytetyzuje wilgotność gleby oraz temperaturę powietrza i uwzględnia natężenie światła wraz z wilgotnością powietrza.

#### Obsługa błędów czujników

Wobec czujników korzystających z **GPIO** lub **ANALOG**, wykrywane są niepożądane stany i nieprawidłowe wartości, czujniki obsługujące magistralę I2C są kontrolowane przed i po wykonaniu odczytów a przypadku błędów, magistrala jest resetowana, a czujniki są ponownie konfigurowane.

### Aplikacja Webowa

#### Funkcjnalności rozszerzające działanie systemu
1. Wykresy danych historycznych
2. Wyświetlanie bieżących danych bezpośrednio w aplikacji
3. Uruchamianie podlewania za pomocą aplikacji

#### Komunikacja z aplikacją

Komunikacja urządzenia z aplikacją odbywa się z wykrozystaniem lokalnego AP, który udostępnia urządzenie z wykrozystaniem protokołu HTTP i formatu JSON.
