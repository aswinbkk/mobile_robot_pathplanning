from django.contrib import admin
from django.urls import path
from django.contrib.auth import views as auth_views
from recognition import views as recog_views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('', recog_views.home, name='home'),
    path('dashboard/', recog_views.dashboard, name='dashboard'),
    path('add_new_face/', recog_views.add_new_face, name='add-new-face'),
    path('add_new_face_instructions/', recog_views.add_new_face_instructions, name='add-new-face-instructions'),
    path('login/',auth_views.LoginView.as_view(template_name='recognition/login.html'),name='login'),
    path('logout/',auth_views.LogoutView.as_view(template_name='recognition/home.html'),name='logout'),
    path('about_us', recog_views.contact_us ,name='contact-us'),
    path('confirm_your_face', recog_views.confirm_your_face ,name='confirm-your-face'),
    path('confirm_your_face_instructions', recog_views.confirm_your_face_instructions ,name='confirm-your-face-instructions')


]
