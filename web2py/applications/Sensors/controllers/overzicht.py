import re

from bokeh.embed import file_html
from bokeh.plotting import figure
from bokeh.resources import CDN
from datetime import datetime as test
import datetime as test2


def index():
    rows = db(db.sensor.id > 0).select()

    db.thresholds.update_or_insert(db.thresholds.type == request.vars['type'], type=request.vars['type'],
                                   value=request.vars['value'])

    formThreshold = SQLFORM(db.thresholds)
    return dict(message="hello from overzicht.py", rijen=rows, formThreshold=formThreshold)


def removeOldClock(sensorId):
    functieNaam = "tijd" + sensorId
    if not db(db.scheduler_task.function_name == functieNaam).isempty():
        db(db.scheduler_task.function_name == functieNaam).delete()


def createSchedule(tijdstip):
    functieNaam = "tijd" + request.vars['naam']
    today = test.today()
    split = tijdstip.split(":")
    today = today.replace(hour=int(split[0]), minute=int(split[1]), second=int(split[2]))
    formatEx = '%H:%M:%S'
    tijdstip = test.strptime(tijdstip, formatEx)
    tomorrow = today
    if test.now().time() > tijdstip.time():
        tomorrow = today + test2.timedelta(days=1)
    mysched.queue_task(functieNaam, start_time=tomorrow, repeats=0, period=86400)


def detail():
    form = FORM(TABLE(TR('Tijdstip:', INPUT(_name='name', requires=IS_TIME())), TR(INPUT(_type='submit'))))
    if form.accepts(request, session):
        removeOldClock(request.vars['naam'])
        tijdstip = request.vars['name']
        createSchedule(tijdstip)
        response.flash = T("Wekker staat aan")
    elif form.errors:
        response.flash = T("Fout ingegeven")

    sensorNaam = request.vars['naam']
    timeString = db(db.data.sensor == sensorNaam).select(db.data.tijd).as_list()
    tempString = db(db.data.sensor == sensorNaam).select(db.data.temperatuur).as_list()

    temp = []
    time = []

    for var in tempString:
        waarde = re.search(r"\d+", str(var)).group()
        temp.append(waarde)

    for var in timeString:
        waarde = re.search(r"\d+", str(var)).group()
        # waarde= (tm.strftime('%Y-%m-%d %H:%M:%S', tm.gmtime(float(waarde))))
        time.append(waarde)

    plot = figure(x_axis_type="datetime")
    plot.line(time, temp)

    html = file_html(plot, CDN, "my plot")
    return dict(bokeh_script=html, name=sensorNaam, form=form)
