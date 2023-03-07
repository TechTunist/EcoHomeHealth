from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name='home'),
    path('live', views.liveStream, name='live'),
    path('lobby', views.lobby)
]