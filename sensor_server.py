from flask import Flask, request, render_template, Response
from datetime import datetime

app = Flask(__name__)

# Create just a single route to read data from our ESP32
@app.route('/data', methods = ['GET'])
def addData():
    ''' The one and only route. It extracts the
    data from the request, converts to float if the
    data is not None, then calls the callback if it is set
    '''
    global _callback_

    date = datetime.now()
    windSpeedStr = request.args.get('windSpeed')
    bmpTempStr = request.args.get('bmpTemp')
    bmpPressureStr = request.args.get('bmpPressure')
    dhtTempStr = request.args.get('dhtTemp')
    dhtHumidityStr = request.args.get('dhtHumidity')

    windSpeed = float(windSpeedStr) if windSpeedStr else None
    bmpTemp = float(bmpTempStr) if bmpTempStr else None
    bmpPressure = float(bmpPressureStr) if bmpPressureStr else None
    dhtTemp = float(dhtTempStr) if dhtTempStr else None
    dhtHumidity = float(dhtHumidityStr) if dhtHumidityStr else None

    time = date.strftime("%H:%M:%S")

    print(f"{time}, {windSpeed:.2f}, {bmpTemp:.2f}, {bmpPressure}, {dhtTemp:.2f}, {dhtHumidity:.2f}\n")
    
    return "OK", 200

def main():
    app.run(host = "0.0.0.0", port = '3237')


if __name__ == '__main__':
    main()
