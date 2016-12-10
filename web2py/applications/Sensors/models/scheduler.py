from gluon.scheduler import Scheduler
import time
import calendar


def temp1():
    timeStamp = calendar.timegm(time.gmtime())
    # zoeken in db naar adres van sensor en dan script gebruiken voor temp op te vragen
    sensor = "sensor1"

    test = db(db.thresholds.type == "Temperatuur").select(db.thresholds.value)
    value = test[0].value

    temp = 12
    if int(value) < temp:
        print("Te hoge temperatuur")

    rows = db(db.sensor.naam == sensor).select()
    row = rows[0]
    db.data.insert(sensor=row.id, temperatuur=temp, tijd=timeStamp)
    db.commit()


def tijd1():
    # zoek in db naar adres voor sensor
    response.flash = T("Klok gaat af")
    print("Script")


mysched = Scheduler(db, tasks=dict(temp1=temp1, tijd1=tijd1))
