# Catatan Setup & Panduan Perakitan

Dokumen ini berisi panduan teknis langkah demi langkah untuk merakit dan menyiapkan perangkat keras Proyek IoT Jamur.

---

## 1. Analogi & Alur Daya 💡

Untuk mempermudah, bayangkan sistem ini seperti tubuh:

- **Power Supply 12V**: Jantung, sumber daya utama.
- **Buck Converter**: Ginjal, mengubah daya 12V menjadi 5V yang aman.
- **ESP32**: Otak, pusat kontrol dan logika.
- **LCD, Sensor**: Mata dan indera.
- **Pompa**: Otot yang bekerja.
- **Relay**: Katup yang dikontrol otak untuk mengalirkan daya ke otot.
- **Ground (GND)**: Sistem peredaran darah balik. **Semua GND wajib terhubung menjadi satu**.

---

## 2. Pemasangan Tombol Power Utama (SDL16-22ADL)

Ini adalah langkah pertama dan paling krusial. Tujuannya adalah memutus aliran listrik 220V ke Power Supply.

> **⚠️ PERINGATAN KESELAMATAN!** Pastikan steker TERCABUT dari stopkontak selama proses ini. Bekerja dengan listrik 220V sangat berbahaya.

**Identifikasi Pin:**

- **Saklar**: Cari pin berlabel `C` (Common) dan `NO` (Normally Open).
- **Lampu LED**: Cari pin berlabel `+` dan `-`.

**Langkah-langkah:**

1.  Ambil kabel listrik AC (kabel steker).
2.  Potong **salah satu** kabelnya (misal, yang berwarna coklat/hitam).
3.  Hubungkan kedua ujung potongan ke pin `C` dan `NO` pada tombol.
4.  Hubungkan kaki `+` LED pada tombol ke output **+12V** Power Supply.
5.  Hubungkan kaki `-` LED pada tombol ke output **GND (-)** Power Supply.

**Diagram:**

```
Stop Kontak 220V --- [Kabel Fasa] --- ● (C)  (NO) ● ---> Input Fasa PSU
                                     |  TOMBOL  |
Stop Kontak 220V --- [Kabel Netral]-------------------> Input Netral PSU
```

---

## 3. Panduan Pengkabelan Lengkap

#### **Langkah A: Distribusi Daya 12V & 5V**

1.  **Atur Buck Converter**: Sebelum menghubungkan ke mana pun, colokkan Power Supply, nyalakan sistem, dan ukur output Buck Converter dengan multimeter. Putar potensiometer kecilnya hingga output stabil di **5V**. Matikan sistem lagi.
2.  **Jalur 12V**: Hubungkan output **+12V** dari Power Supply ke:
    - Pin `IN+` pada Buck Converter.
    - Terminal `COM` pada Modul Relay.
3.  **Jalur GND Utama**: Hubungkan output **GND (-)** dari Power Supply ke:
    - Pin `IN-` pada Buck Converter.
    - Kabel **negatif (-)** dari Pompa Air.
4.  **Jalur 5V**: Hubungkan `OUT+` (5V) dari Buck Converter ke pin berikut secara **paralel** (gunakan terminal blok atau solder jadi satu):
    - Pin `VIN` pada ESP32.
    - Pin `VCC` pada LCD I2C.
    - Pin `VCC` pada Modul Relay.
5.  **Jalur GND Logika**: Hubungkan `OUT-` (GND) dari Buck Converter ke salah satu pin `GND` pada ESP32. Ini akan menjadi titik pusat ground untuk semua komponen logika.

#### **Langkah B: Sirkuit Beban & Sinyal**

1.  **Pompa**: Hubungkan terminal `NO` (Normally Open) pada Relay ke kabel **positif (+)** Pompa Air.
2.  **Relay Kontrol**: Hubungkan pin `IN` pada Relay ke `GPIO 23` ESP32. Hubungkan `GND` Relay ke `GND` ESP32.
3.  **LCD I2C**: Hubungkan `SDA` ke `GPIO 21`, `SCL` ke `GPIO 22`, dan `GND` ke `GND` ESP32.
4.  **Sensor DHT11**: Hubungkan `DATA` ke `GPIO 4`, `VCC` ke pin **`3V3`** ESP32, dan `GND` ke `GND` ESP32.
5.  **Tombol**: Hubungkan satu kaki dari setiap tombol ke pin GPIO yang sesuai (`19, 18, 5, 17`). Hubungkan kaki lainnya dari **semua tombol** menjadi satu ke pin `GND` ESP32.

---

## 4. Konfigurasi Awal Perangkat

1.  **Upload Kode**: Pastikan Anda sudah mengunggah kode dari 3 file (`.ino`, `config.h`, `functions.h`) ke ESP32.
2.  **Setup WiFi (Pertama Kali)**:
    - Nyalakan perangkat. LCD akan menampilkan "Mode Setup WiFi".
    - Gunakan HP atau laptop Anda, cari jaringan WiFi bernama **`JamurIoT_Setup`**.
    - Hubungkan ke jaringan tersebut dengan password **`jamur123`**.
    - Buka browser dan kunjungi alamat `http://192.168.4.1`.
    - Masukkan Nama (SSID) dan Password WiFi rumah Anda, lalu klik "Simpan & Reboot".
3.  **Selesai!**: Perangkat akan restart dan secara otomatis terhubung ke WiFi Anda dan mulai beroperasi normal.

Untuk masuk kembali ke mode setup, cukup tahan tombol **KEMBALI** saat perangkat dinyalakan.
