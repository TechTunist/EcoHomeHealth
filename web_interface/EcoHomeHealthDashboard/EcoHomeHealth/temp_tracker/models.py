from django.db import models
from datetime import datetime

class DH11Sensor(models.Model):
    temperature = models.FloatField(null=True, blank=True)
    humidity = models.FloatField(null=True, blank=True)
    location = models.CharField(max_length=20, null=True, blank=True, default='My Office Desk')
    time_of_record = models.DateTimeField(default=datetime.now)

    def __str__(self):
        return self.location

