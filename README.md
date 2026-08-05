Download manager<br/>
<br/>
Download Manager with the following capabilities:<br/>
- Create queues for downloads<br/>
- Schedule to start and stop queues<br/>
- Stop and resume download<br/>
- Multi Thread</br>
- Add sequential Downloads<br/>
- Move completed download by drag and drop from Competed download dialog<br/>
- Show completed download in folder<br/>
- Open completed download with desired program (Open with...)<br/>
- launching program on startup<br/>
- Add Proxy(Sock5,Http)<br/>
- Change program language<br/>
- ...

<br/>
<br/>
<br/>

<div dir="rtl">
دانلود منیجر<br/>
دانلود منیجر با قابلیت های زیر:<br/>
- ساخت صف ها برای دانلود ها<br/>
-زمان بندی برای شروع و توقف صف ها<br/>
-توقف و ادامه صف ها<br/>
-چند نخی (پردازش موازی) </br>
-افزودن دانلود های متوالی<br/>
-انتقال دانلود تکمیل شده توسط کشیدن و رها کردن در صفحه نمایش اتمام دانلود<br/>
-نمایش دانلود تکمیل شده در فولدر<br/>
-بازکردن دانلود تکمیل شده توسط برنامه دلخواه(Open with...)<br/>
-اجرای برنامه در startup<br/>
-افزودن پروکسی (Socks5,Http)<br/>
-تغییر زبان برنامه<br/>
-...<br/>
</div>
<br/>
<br/>


![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/1.jpg?raw=true)
<br/>
<br/>
![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/2.jpg?raw=true)
<br/>
<br/>
![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/3.jpg?raw=true)
<br/>
<br/>
![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/4.jpg?raw=true)
<br/>
<br/>
![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/5.jpg?raw=true)
<br/>
<br/>
![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/6.jpg?raw=true)
<br/>
<br/>
![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/7.jpg?raw=true)
<br/>
<br/>
![alt text](https://github.com/hamedshakib/New-Download-Manager/blob/develop/Images/8.jpg?raw=true)

<br/>
<br/>

## نحوه بیلد / Build Instructions

این پروژه از CMake Presets استفاده می‌کند و **هیچ مسیر شخصی/سیستم‌مخصوصی** در فایل‌های مشترک وجود ندارد. مسیر Qt از متغیر محیطی `QT_ROOT` خوانده می‌شود؛ بنابراین روی هر کامپیوتری که Qt نصب است قابل بیلد است.

### ۱) تنظیم متغیر محیطی QT_ROOT (فقط یک‌بار در هر سیستم)

این متغیر باید به پوشه‌ی Qt مخصوص همان کامپیوتر اشاره کند (پوشه‌ای که شامل پوشه‌های `bin`, `lib`, `include` و `lib/cmake` است).

**Windows (پاورشل):**
```
setx QT_ROOT "C:/Qt/6.11.1/msvc2022_64"
```
(پس از setx، ترمینال را ببندید و دوباره باز کنید تا متغیر بارگذاری شود.)

**Linux/macOS:**
```bash
export QT_ROOT=/opt/Qt/6.11.1/gcc_64   # و این خط را به ~/.bashrc یا ~/.zshrc اضافه کنید
```

### ۲) بیلد

- **از طریق IDE (VS Code / Visual Studio / Qt Creator):** فقط یک پریست (مثلاً `x64-Debug`) را انتخاب کنید؛ بقیه خودکار انجام می‌شود.
- **از خط فرمان:**
  ```
  cmake --preset x64-Debug
  cmake --build --preset x64-Debug
  ```

> نکته: `CMakeUserPresets.json` یک فایل شخصی است و عمداً در کنترل نسخه نیست (در `.gitignore` است). هر توسعه‌دهنده می‌تواند پریست شخصی خودش را در آن قرار دهد.
