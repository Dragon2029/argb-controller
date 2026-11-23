<div align="center">

<img src="https://github.com/user-attachments/assets/43d79049-ab8c-4f8e-a674-e3c5f2038e6e" alt="ARGB Controller" width="120"/>


# 🎨 ARGB Controller

**برنامج تحكم كامل في إضاءة ARGB للمراوح عبر Arduino Leonardo**

[![Python](https://img.shields.io/badge/Python-3.9+-3776AB?style=flat&logo=python&logoColor=white)](https://www.python.org/)
[![PyQt6](https://img.shields.io/badge/PyQt6-6.x-41CD52?style=flat&logo=qt&logoColor=white)](https://pypi.org/project/PyQt6/)
[![Arduino](https://img.shields.io/badge/Arduino-Leonardo-00979D?style=flat&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![FastLED](https://img.shields.io/badge/FastLED-Library-orange?style=flat)](https://fastled.io/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=flat&logo=windows&logoColor=white)]()

</div>

---

## 📖 نظرة عامة
**ARGB Controller**

هو مشروع مفتوح المصدر يهدف للتحكم الكامل في إضاءة المراوح (ARGB Fans) عبر لوحة **Arduino Leonardo**، مع واجهة سطح مكتب احترافية مبنية بـ **Python + PyQt6**.

يدعم البرنامج **12 وضع إضاءة**، **تفاعل صوتي مباشر** (ميكروفون + صوت الـ Desktop)، **لوحة ألوان تفاعلية بشكل خلية نحل**، **تتالي ألوان تلقائي مع انتقال سلس**، وحفظ الإعدادات على الـ **EEPROM** داخل الأردوينو.

> 🧑‍💻 **المطور:** المهندس / فؤاد محمد
> 📦 **الإصدار:** v1.0

---

## ✨ المميزات

### 🎛️ التحكم والإضاءة
- ✅ **12 وضع إضاءة** (ثابت، قوس قزح، تنفس، تفاعل صوتي، شهاب، نجوم، موجة، إطفاء، ذهاب وإياب، نبض مزدوج، مسح لوني، ستروب).
- ✅ **تحكم كامل في السطوع** (0-255).
- ✅ **سرعة تأثيرات قابلة للتعديل** (1-100%).
- ✅ **حساسية صوت قابلة للضبط** (1x - 20x).
- ✅ **دعم عدد ليدات قابل للتغيير** (حتى 100 LED).

### 🎨 الألوان
- ✅ **لوحة ألوان خلية نحل تفاعلية** (Honeycomb Picker) — كليك يسار = تثبيت، كليك يمين = إضافة للتتالي.
- ✅ **9 ألوان جاهزة** في قائمة سريعة.
- ✅ **لون مخصص** عبر نافذة اختيار متقدمة.
- ✅ **تتالي ألوان** قابل للبرمجة (Sequence) مع:
  - إضافة ألوان مخصصة أو من القائمة.
  - حذف / مسح الكل.
  - تشغيل تلقائي بفاصل زمني قابل للتعديل.
  - **انتقال سلس** بين الألوان (Fade).
  - خيار الحفاظ على الوضع الحالي (بدون رجوع للثابت).

### 🔊 التفاعل الصوتي
- ✅ **مصدر صوت اختياري:** ميكروفون أو **Desktop Audio (Loopback)**.
- ✅ **مؤشرات Bass / Treble مباشرة** في الواجهة.
- ✅ يعمل عبر `sounddevice` (ميكروفون) و `pyaudiowpatch` (Desktop).

### 🖥️ تجربة المستخدم
- ✅ **واجهة عربية كاملة** (RTL) بتصميم داكن عصري.
- ✅ **System Tray** مع قائمة كاملة للتحكم من خارج النافذة.
- ✅ **إغلاق ذكي:** زر X يصغّر البرنامج للـ Tray بدل قفله.
- ✅ **اكتشاف تلقائي للأردوينو** (يُميّز Arduino Leonardo بنجمة ★).
- ✅ **حفظ واسترجاع الإعدادات** في ملف JSON.
- ✅ **معايرة عدد الليدات** بسهولة من داخل البرنامج.

---

## 🗂️ هيكل المشروع

argb-controller/

│

├──ARGB_Controller_Setup_v1.0.exe

├── arduino/

│ └── argb_firmware.ino # سكيتش Arduino (FastLED + EEPROM)

├── images

│ └── argb_wiring.png # سكيتش Arduino (FastLED + EEPROM)


---


## 🚀 التثبيت والتشغيل
1. ثبت البرنامج على الويندوز بالطريقة العادية و بعد ذالك افتح البرنامج و حدد المنفذ و اضغط اتصال
### 1️⃣ تجهيز الأردوينو

**المتطلبات:**
- Arduino Leonardo (أو أي لوحة تدعم `Keyboard`/`Mouse` HID — موصى بها للاستقرار).
- مكتبة **FastLED** (ثبّتها من Arduino IDE → Library Manager).

**الخطوات:**
1. افتح ملف `arduino/argb_firmware.ino` في Arduino IDE.
2. عدّل عدد الليدات في السكيتش إذا لزم الأمر:
   ```cpp
   const int fanLedCounts[NUM_FANS] = {12, 18, 15, 15};
4.رفع السكيتش على اللوحة.

5. صّل خط Data لمرواح ARGB بمنفذ D6 على الأردوينو (مع مقاومة 330Ω ومكثف 1000µF للاستقرار).

🎮 طريقة الاستخدام

العنصر	الوظيفة
المنفذ (COM)	يكتشف المنافذ تلقائيًا ويميز الأردوينو بـ ★
اتصال / قطع الاتصال	يفتح/يغلق الاتصال التسلسلي (ينتظر 2 ثانية للـ Reset)
لوحة خلية النحل	كليك يسار = تثبيت اللون / كليك يمين = إضافة للتتالي
أزرار الأوضاع	تشغيل أحد الـ 12 وضع مباشرة
تبديل تلقائي	يتنقل بين الأوضاع المفضلة كل X ثانية
تتالي الألوان	يشغل سلسلة ألوان مع انتقال سلس
مؤشرات الصوت	تعرض Bass/Treble لحظيًا (للتفعيل: اختر وضع "تفاعل صوتي")
حفظ على الأردوينو	يثبّت الإعدادات في EEPROM
💡 اختصارات
كليك يسار على أيقونة الـ Tray → إظهار/إخفاء النافذة.

كليك يمين على أيقونة الـ Tray → قائمة كاملة بالتحكم.

زر X → تصغير للـ Tray (قابل للتعطيل من الإعدادات).

🔌 بروتوكول الاتصال (Serial)
البرنامج يرسل أوامر نصية بسيطة عبر Serial (115200 baud). كل أمر ينتهي بـ \n.

الأمر	الوصف	مثال
C:r,g,b	تعيين اللون	C:255,0,0
M:n	تعيين الوضع (0-11)	M:1
B:n	السطوع (0-255)	B:200
S:n	السرعة (1-100)	S:50
N:n	عدد الليدات	N:60
F:b,t	بيانات صوت (Bass, Treble)	F:120,80
K:r,g,b	تغيير اللون بدون تغيير الوضع	K:0,255,0
CAL	بدء معايرة عدد الليدات	CAL
SAVE	حفظ الإعدادات في EEPROM	SAVE
ردود الأردوينو:

الرد	المعنى
LED:n	رقم الليد الحالي أثناء المعايرة
N_OK:n	تم ضبط عدد الليدات
SAVED_OK	تم الحفظ في EEPROM
🛠️ التقنيات المستخدمة
التقنية	الاستخدام
Python 3.9+	لغة البرمجة الرئيسية
PyQt6	واجهة المستخدم
pyserial	الاتصال التسلسلي مع الأردوينو
sounddevice	التقاط الصوت من الميكروفون
pyaudiowpatch	التقاط صوت الـ Desktop (Windows)
numpy	معالجة الإشارات (FFT)
FastLED	التحكم في ليدات WS2812B
Arduino Leonardo	لوحة التحكم الرئيسية
🖼️ لقطات الشاشة

<img width="234" height="114" alt="image" src="https://github.com/user-attachments/assets/dbbb7593-c730-49b6-aaf2-a69a8009f822" /> <img width="522" height="892" alt="image" src="https://github.com/user-attachments/assets/32692c4c-d1fe-44fe-9b89-eb47d0cac55f" /> <img width="522" height="892" alt="image" src="https://github.com/user-attachments/assets/f510822f-aaf4-47b1-bae3-fc79bebb013f" />



---

## 🔌 التوصيلات (Wiring Diagram)

قبل تشغيل البرنامج، تأكد من توصيل الأردوينو بمنفذ الـ ARGB في المروحة بالشكل التالي:

<img width="1214" height="809" alt="ARGB Connection" src="https://github.com/user-attachments/assets/ad4ed69a-9ce2-46c2-bb83-307bb19dd23d" />


### 📋 خطوات التوصيل:

| طرف المروحة (ARGB) | طرف الأردوينو (Arduino Leonardo) | ملاحظات |
| +5v | ARGB | :--- | GND |
| **GND** (السلك الأسود) | **GND** (أي منفذ GND) | ضروري لتوحيد الأرضي بين الأردوينو والمروحة |
| **Data** (سلك البيانات) | **D6** (Digital Pin 6) | يمكن تغييرها في الكود (`#define LED_PIN 6`) |
| **5V** (السلك الأحمر) | **غير موصول (Not Connected)** ⚠️ | **لا توصله بالأردوينو!** |

> ⚠️ **تحذير هام:** 
> **لا تقم بتوصيل سلك الـ 5V الخاص بالمروحة بمنفذ الـ 5V في الأردوينو.** الأردوينو لا يتحمل تيار المراوح والإضاءة العالي، وقد يحترق. المروحة يجب أن تأخذ كهربائها من مصدر الطاقة الخاص بها (Power Supply أو المذربورد)، ويتم توصيل سلك الـ Data والـ GND فقط بالأردوينو.
> 
> يُفضل إضافة **مقاومة 330 أوم** على سلك الداتا لحماية الأردوينو، و**مكثف 1000 ميكروفاراد** بين الـ 5V والـ GND (قريب من المروحة) لتثبيت الإضاءة ومنع الوميض.




