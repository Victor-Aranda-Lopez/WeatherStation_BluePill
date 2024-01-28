import sqlite3
import datetime
from peewee import *
#~/Proyectos/WeatherStationServer/measures.db
db = SqliteDatabase('XXXXX/measures.db')

class Measure(Model):
    humidity = FloatField()
    temperature = FloatField()
    dateAndTime = DateTimeField(default=datetime.datetime.now)

    class Meta:
        database = db # This model uses the "people.db" database.

if __name__ == "__main__":
    db.connect()
    db.create_tables([Measure], safe = True)
    mea= Measure.create(humidity=2.3,temperature=20)
    db.close()