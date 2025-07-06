# Proyek IoT Smart Kumbung Jamur Tiram

**Versi Firmware: 18.0**

Selamat datang di dokumentasi Proyek IoT Smart Kumbung Jamur Tiram. Proyek ini bertujuan untuk menciptakan sebuah sistem kontrol dan monitoring otomatis untuk budidaya jamur tiram, memastikan kondisi ideal untuk pertumbuhan optimal dan meminimalkan intervensi manual.

Perangkat ini dirancang agar mandiri, dengan kemampuan konfigurasi langsung dari perangkat keras (LCD & tombol) serta dari jarak jauh melalui antarmuka web dan MQTT.

---

## Fitur Utama ✨

- **Kontrol Kelembapan Otomatis**: Menjaga kelembapan dalam rentang ideal dengan menyalakan pompa pengabutan secara otomatis.
- **Penyiraman Terjadwal**: Melakukan penyiraman rutin pada jam-jam yang telah ditentukan.
- **Monitoring Real-time**: Mengirim data suhu dan kelembapan setiap beberapa detik ke broker MQTT.
- **Konfigurasi Mandiri**: Dilengkapi mode **Access Point** dengan **Web Server** untuk mengatur koneksi WiFi tanpa perlu mengubah kode.
- **Konfigurasi Jarak Jauh (Remote)**: Pengaturan ambang batas kelembapan dan jadwal dapat diubah melalui pesan MQTT dari frontend.
- **Antarmuka Fisik**: Dilengkapi layar **LCD 16x2** untuk menampilkan status dan **4 tombol** untuk interaksi & kontrol manual.
- **Sistem Notifikasi Cerdas**: Mengirimkan notifikasi ke MQTT untuk setiap kejadian penting, mulai dari aksi pompa, peringatan, hingga status perangkat.

---

## Daftar Komponen 🛠️

| Komponen                     | Spesifikasi / Tipe                            | Fungsi Utama                                                |
| :--------------------------- | :-------------------------------------------- | :---------------------------------------------------------- |
| **Mikrokontroler**           | ESP32 DEVKIT V1                               | Otak utama pemrosesan, WiFi, dan kontrol.                   |
| **Sensor Suhu & Kelembapan** | DHT11 (atau DHT21/AM2301)                     | Membaca kondisi suhu dan kelembapan.                        |
| **Layar Display**            | LCD 16x2 dengan Modul I2C                     | Menampilkan status perangkat, data sensor, dan menu.        |
| **Tombol Input**             | 4x Push Button (Keypad 4x1)                   | Navigasi menu dan kontrol manual.                           |
| **Power Supply Utama**       | Adaptor DC 12V (rekomendasi min. 5A)          | Sumber daya utama untuk seluruh sistem.                     |
| **Pompa Air DC**             | Sakai 12V 3.5A (atau sejenis)                 | Memompa air untuk sistem pengabutan.                        |
| **Modul Relay**              | 1 Channel 5V Relay Module                     | Saklar elektronik untuk mengontrol pompa 12V dari ESP32.    |
| **Penurun Tegangan**         | Buck Converter DC-DC (e.g., LM2596)           | Menurunkan tegangan dari 12V ke 5V untuk ESP32 & logika.    |
| **Tombol Power**             | Latching Push Button 16mm (e.g., SDL16-22ADL) | Saklar utama untuk menyalakan dan mematikan seluruh sistem. |
| **Lain-lain**                | Box Proyek, Kabel Jumper, Sprayer, Selang     | Komponen pendukung untuk perakitan.                         |

---

## Arsitektur Sistem

Sistem ini bekerja dengan alur sebagai berikut:

1.  **Sensor** membaca data lingkungan.
2.  **ESP32** memproses data dan menjalankan logika (otomatis, terjadwal, manual).
3.  **Aktuator** (Relay & Pompa) dieksekusi berdasarkan perintah dari ESP32.
4.  **LCD & Tombol** menyediakan antarmuka lokal untuk monitoring dan kontrol dasar.
5.  **MQTT** menjadi jembatan komunikasi dua arah antara perangkat dan frontend untuk monitoring dan kontrol jarak jauh.
6.  **Web Server** (dalam mode AP) berfungsi sebagai portal untuk konfigurasi awal koneksi WiFi.
