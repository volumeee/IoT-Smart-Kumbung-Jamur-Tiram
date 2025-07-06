# Daftar Fitur & Contoh Data - Firmware Jamur IoT v18.0

Dokumen ini merinci semua fitur fungsional yang tertanam dalam firmware perangkat IoT Jamur, beserta contoh payload data MQTT yang dikirimkan untuk setiap fitur.

---

## I. Konektivitas & Konfigurasi Mandiri 🌐

Fitur-fitur ini memastikan perangkat dapat terhubung dan dikonfigurasi dengan mudah tanpa perlu mengunggah ulang kode.

- **1. WiFi Manager dengan Access Point**

  - Jika perangkat tidak memiliki kredensial WiFi atau jika tombol `KEMBALI` ditahan saat boot, perangkat akan membuat jaringan WiFi sendiri bernama **`JamurIoT_Setup`**. Pengguna dapat terhubung ke AP ini dan membuka `http://192.168.4.1` dari browser untuk memasukkan SSID dan Password WiFi rumah.

- **2. Penyimpanan Kredensial Permanen**

  - Kredensial WiFi dan pengaturan konfigurasi lainnya (ambang batas, jadwal) disimpan di memori flash NVS (EEPROM) menggunakan library `Preferences`. Pengaturan ini tidak akan hilang meskipun perangkat mati listrik.

- **3. Koneksi Aman ke MQTT Broker**

  - Semua komunikasi dengan broker MQTT menggunakan koneksi terenkripsi **TLS/SSL (MQTTS)** pada port `8883` dan memerlukan autentikasi **username & password**, memastikan data aman saat transit.

- **4. LWT (Last Will and Testament)**
  - Jika perangkat terputus dari jaringan secara tiba-tiba (misal, mati listrik), broker MQTT akan secara otomatis mengirimkan pesan status terakhir ke topik `jamur/status`.
  - **Contoh Data MQTT:**
    ```json
    // Topik: jamur/status
    { "state": "offline" }
    ```

---

## II. Kontrol & Otomatisasi Cerdas 🧠

Fitur inti yang menjalankan logika budidaya secara otomatis.

- **1. Kontrol Kelembapan Otomatis Bertingkat**

  - Perangkat secara aktif menjaga kelembapan dengan tiga level logika:
    - **Zona Kritis**: Jika kelembapan di bawah `h_crit` (misal, 80%), pompa akan dipaksa menyala.
    - **Zona Peringatan**: Jika kelembapan di bawah `h_warn` (misal, 85%), perangkat akan mengirim notifikasi peringatan.
    - **Zona Aman**: Jika kelembapan kembali normal, perangkat akan mengirim notifikasi info.

- **2. Penyiraman Terjadwal dengan Prioritas**

  - Pompa akan menyala secara otomatis pada jam-jam yang telah ditentukan dalam konfigurasi.
  - **Logika Cerdas**: Penjadwalan ini **tidak akan berjalan** jika kondisi kelembapan sudah berada di zona kritis untuk menghindari penyiraman berlebih.

- **3. Kontrol Manual (Triple-Access)**
  - Pompa dapat dinyalakan secara manual melalui tiga cara:
    1.  **MQTT**: Mengirim pesan `ON` ke topik `jamur/control/pump`.
    2.  **Tombol Fisik**: Menahan tombol `OK` pada perangkat selama >1.5 detik.
    3.  **Frontend**: Melalui tombol di website Anda (yang pada akhirnya mengirim perintah MQTT).
  - **Contoh Data MQTT saat Aksi Manual:**
    ```json
    // Topik: jamur/notification (jika dari tombol fisik)
    {
      "type": "info",
      "message": "Pompa dinyalakan (manual_fisik).",
      "humidity": 88.1,
      "temperature": 27.5
    }
    ```

---

## III. Monitoring & Pelaporan Real-time 📊

Fitur untuk memantau kondisi perangkat dan lingkungan dari jarak jauh.

- **1. Telemetri Suhu & Kelembapan**

  - Mengirim data sensor secara periodik (diatur di `LOGIC_CHECK_INTERVAL_MS`).
  - **Contoh Data MQTT:**
    ```json
    // Topik: jamur/telemetry
    {
      "temperature": 27.8,
      "humidity": 89.5
    }
    ```

- **2. Laporan Status Perangkat**

  - Memberikan status operasional perangkat secara real-time.
  - **Contoh Data MQTT:**
    ```json
    // Topik: jamur/status
    {"state":"online"}
    {"state":"pumping"}
    {"state":"idle"}
    ```

- **3. Log Notifikasi Berkonteks**

  - Setiap kejadian penting dicatat dan dikirim dengan data sensor saat itu juga.
  - **Contoh Data MQTT:**
    ```json
    // Topik: jamur/notification
    {
      "type": "info",
      "message": "Pompa telah berhenti.",
      "humidity": 92.5,
      "temperature": 27.2
    }
    ```

- **4. Monitoring Sinyal WiFi (RSSI)**
  - Secara periodik melaporkan kekuatan sinyal WiFi yang diterima perangkat. Pesan ini bersifat _retained_.
  - **Contoh Data MQTT:**
    ```json
    // Topik: jamur/wifi_signal
    { "rssi": -62 }
    ```

---

## IV. Konfigurasi Jarak Jauh (Remote Configuration) ⚙️

Fitur paling canggih yang memungkinkan penyesuaian perilaku perangkat tanpa kontak fisik.

- **1. Update Pengaturan via MQTT**

  - Frontend dapat mengirim pesan JSON ke topik `jamur/config/set` untuk mengubah parameter seperti ambang batas kelembapan dan jadwal.

- **2. Konfirmasi Pengaturan (Two-way)**
  - Setelah menerima konfigurasi baru, perangkat akan menyimpannya ke memori permanen, lalu mempublikasikan **seluruh konfigurasi barunya** ke topik `jamur/config/get` sebagai tanda konfirmasi. Topik ini bersifat _retained_.
  - **Contoh Alur Data MQTT:**
    1.  **Frontend mengirim perintah:**
        ```json
        // Publish ke topik: jamur/config/set
        {
          "h_warn": 88.0,
          "schedules": [8, 13, 18]
        }
        ```
    2.  **Perangkat merespons dengan konfigurasi baru:**
        ```json
        // Pesan baru di topik: jamur/config/get
        {
          "h_crit": 80.0,
          "h_warn": 88.0,
          "schedules": [8, 13, 18]
        }
        ```

---

## V. Antarmuka Fisik (Lokal) 📟

Fitur yang memungkinkan interaksi langsung dengan perangkat tanpa memerlukan koneksi internet atau aplikasi.

- **1. Tampilan LCD Informatif**

  - Menampilkan data suhu, kelembapan, status pompa (ON/OFF), dan status koneksi MQTT (OK/ERR) secara real-time. Layar diperbarui setiap beberapa detik.

- **2. Menu Info Perangkat**

  - Dengan menekan tombol `OK` sesaat, layar akan beralih untuk menampilkan informasi jaringan perangkat, seperti Alamat IP. Tekan tombol `KEMBALI` untuk kembali ke layar utama.

- **3. Kontrol Pompa Manual Fisik**

  - Dengan **menahan tombol `OK` selama lebih dari 1.5 detik**, pengguna dapat langsung menyalakan pompa secara manual untuk satu siklus penyiraman. Sangat berguna untuk pengujian atau penyiraman darurat.

- **4. Mode Setup WiFi Fisik**
  - Dengan **menahan tombol `KEMBALI` saat perangkat dinyalakan**, pengguna dapat memaksa perangkat untuk masuk ke mode **Access Point (AP)**. Ini memungkinkan konfigurasi ulang koneksi WiFi bahkan jika password WiFi lama sudah tidak valid.
