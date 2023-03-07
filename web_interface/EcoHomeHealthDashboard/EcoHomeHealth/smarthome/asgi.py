"""
ASGI config for smarthome project.

It exposes the ASGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/4.0/howto/deployment/asgi/
"""

import os
from django.core.asgi import get_asgi_application
from channels.routing import ProtocolTypeRouter, URLRouter
from channels.auth import AuthMiddlewareStack
import temp_tracker.routing

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'smarthome')

application = ProtocolTypeRouter({
    'http': get_asgi_application(),
    'websocket': AuthMiddlewareStack(
        URLRouter(
            temp_tracker.routing.websocket_urlpatterns
        )
    )
})