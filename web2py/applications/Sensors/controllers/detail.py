import matplotlib.pyplot as plt


def index():
    sensorNaam = request.vars['naam']
    rows = db(db.data.sensor.naam == sensorNaam).select()
    #plt.plot(rows)
    #plt.ylabel('some numbers')
    #plt.show()
    return dict(hello="Hello")
