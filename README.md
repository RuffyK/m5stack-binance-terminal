<style>
    .d-flex {
        display: flex;
    }
    .progress-panel {
        display: flex;
        align-items: center;
        justify-content: flex-end;
        width: 20%;
        position: relative;
        border-right: 3px solid #168aad;
        padding-right: 24px;
        font-weight: bold;
        color: #1e6091;
    }
    .content-panel {
        padding: 1rem;
        padding-left: 24px;
        width: 80%
    }
    .content-panel h3 {
        color: #52b69a;
    }
    .progress-dot {
        border-radius: 50%;
        width: 18px;
        height: 18px;
        background-color: #168aad;
        border: 3px solid #99d98c;
        position: absolute;
        right: -13px;
    }
    .progress-dot.done {
        background-color: #99d98c;
        border: 3px solid #168aad;
    }
</style>

# M5Stack Binance Terminal
Access binance using your ESP32 based M5 Stack Core

## Roadmap


<div class="d-flex">
    <div class="progress-panel">
        <span>July 2022</span>
        <div class="progress-dot done"></div>
    </div>
    <div class="content-panel">
        <h3>Exploration</h3>
        <p>Explore what's possible using the M5Stack and the Binance API</p>
    </div>
</div>

<div class="d-flex">
    <div class="progress-panel">
        <span>August 2022</span>
        <div class="progress-dot"></div>
    </div>
    <div class="content-panel">
        <h3>Plots, Subplots & Indicators</h3>
        <ul>
            <li>Add further Plots, such as 15m, 1h, 4h etc...</li>
            <li>Implement first Subplot demonstration to show Volume</li>
            <li>Include further indicators such as EMA</li>
            <li>Improve Plot Scaling</li>
        </ul>
    </div>
</div>


<div class="d-flex">
    <div class="progress-panel">
        <span>September 2022</span>
        <div class="progress-dot"></div>
    </div>
    <div class="content-panel">
        <h3>User Interface for Plots, Subplots & Indicators</h3>
        <p>Make all the available Plots and Indicators accessible for the user to change them on the fly using the M5Stack buttons or the Web-app hosted on the device..</p>
    </div>
</div>
