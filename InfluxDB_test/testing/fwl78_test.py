#!/usr/bin/python3

import urllib.request
from Crypto.Cipher import AES
import base64
from io import StringIO
import json
import time, datetime
from influxdb import InfluxDBClient

#==========================================================
# data is writte to an InfluxDB database running on pi3dev
# here are the setting needed to access that database
#==========================================================

host = "homeserv" # Could also set local ip address
port = 8086
user = "root"
password = "root"
dbname = "lora"
measurement = "test"
# runNo = datetime.datetime.now().strftime("%Y%m%d%H%M")

# Initialize the Influxdb client
client = InfluxDBClient(host, port, user, password, dbname)

# Get a timestamp - the database (or Grafana?) wants UTC
timestamp = datetime.datetime.utcnow().isoformat()

# Generate a datapoint and write it to the database
datapoint = [ {
    "measurement": measurement,
#    "tags": {"runNum": runNo},
    "time": timestamp,
    "fields": {"count":42, "dummy":"-1.23"}
    } ]
# print(datapoint)
status = client.write_points(datapoint)

if status==False:
    print("failed to write database {0}".format(datapoint))

