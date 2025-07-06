# Panduan Frontend: Mengonsumsi MQTT API Jamur IoT

**Versi Firmware Target: 18.0**

Dokumen ini adalah panduan teknis bagi pengembang frontend (Web, Mobile) untuk terhubung, menerima data, dan mengirim perintah ke perangkat IoT Jamur menggunakan protokol MQTT.

---

## 1. Setup & Koneksi MQTT di Frontend 🌐

Untuk berkomunikasi dengan perangkat, aplikasi frontend Anda harus terhubung ke broker MQTT sebagai klien.

### **Library yang Direkomendasikan**

- **MQTT.js**: Ini adalah library standar emas untuk MQTT di lingkungan JavaScript (Browser dan Node.js).
  - Instalasi: `npm install mqtt` atau `yarn add mqtt`.

### **Detail Koneksi**

Sangat penting untuk menggunakan protokol **WebSocket Secure (`wss://`)** saat terhubung dari browser.

- **Broker URL**: `wss://e21436f97e4c46358cda880324a5a6ba.s2.eu.hivemq.cloud:8883/mqtt`
- **Username**: `SECRET_MQTT_USER`
- **Password**: `SECRET_MQTT_PASSWORD`

### **Contoh Kode Koneksi (JavaScript/TypeScript)**

Ini adalah contoh dasar untuk menghubungkan klien frontend Anda menggunakan MQTT.js.

```javascript
import mqtt from "mqtt";

const brokerUrl =
  "wss://e21436f97e4c46358cda880324a5a6ba.s2.eu.hivemq.cloud:8883/mqtt";

const options = {
  clientId: `frontend-dashboard-${Math.random().toString(16).substr(2, 8)}`,
  username: "SECRET_MQTT_USER",
  password: "SECRET_MQTT_PASSWORD",
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000,
};

console.log("Menghubungkan ke broker MQTT...");
const client = mqtt.connect(brokerUrl, options);

client.on("connect", () => {
  console.log("Berhasil terhubung ke Broker MQTT!");

  // Subscribe ke semua topik yang dibutuhkan setelah terhubung
  client.subscribe("jamur/telemetry", { qos: 0 });
  client.subscribe("jamur/status", { qos: 1 });
  client.subscribe("jamur/notification", { qos: 1 });
  client.subscribe("jamur/config/get", { qos: 1 });
  client.subscribe("jamur/wifi_signal", { qos: 0 });
});

client.on("error", (err) => {
  console.error("Koneksi gagal:", err);
  client.end();
});

client.on("reconnect", () => {
  console.log("Mencoba menghubungkan kembali...");
});

// Handler utama untuk semua pesan yang masuk
client.on("message", (topic, payload) => {
  const message = payload.toString();
  console.log(`Pesan diterima di topik [${topic}]:`, message);

  // Di sini Anda akan memproses pesan dan memperbarui state aplikasi Anda
  // (misal: dengan React useState, Redux, Zustand, dll.)
});
```

---

## 2. Menampilkan Data (Subscribe & State Management) 📊

Setelah terhubung, frontend perlu men-_subscribe_ ke topik-topik tertentu dan mengolah data yang masuk untuk ditampilkan ke pengguna.

### **Alur Kerja Pengolahan Data**

Di dalam `client.on('message', (topic, payload) => { ... })`, Anda perlu mem-parsing data dan menyimpannya ke dalam _state management_ aplikasi Anda.

```javascript
// Contoh state management di React
const [telemetry, setTelemetry] = useState({ temperature: 0, humidity: 0 });
const [deviceStatus, setDeviceStatus] = useState("offline");
const [config, setConfig] = useState(null);
const [notifications, setNotifications] = useState([]);

// Di dalam client.on('message', ...)
const data = JSON.parse(payload.toString());

switch (topic) {
  case "jamur/telemetry":
    setTelemetry(data);
    break;
  case "jamur/status":
    setDeviceStatus(data.state);
    break;
  case "jamur/config/get":
    setConfig(data);
    break;
  case "jamur/notification":
    // Tambahkan notifikasi baru ke daftar, tampilkan sebagai toast
    setNotifications((prev) => [data, ...prev].slice(0, 10)); // Simpan 10 notif terakhir
    showToast(data.message, { type: data.type }); // Contoh fungsi toast
    break;
  // ... dan seterusnya
}
```

---

## 3. Mengirim Perintah & Konfigurasi (Publish) ⚙️

Frontend dapat mengontrol dan mengonfigurasi perangkat dengan mem-publikasikan pesan ke topik tertentu.

### **Kontrol Manual Pompa**

- **Topik**: `jamur/control/pump`
- **Payload**: String `ON`
- **Contoh Kode**:
  ```javascript
  function handleManualPump() {
    if (client && client.connected) {
      client.publish("jamur/control/pump", "ON", { qos: 1 });
      showToast("Perintah siram manual terkirim!");
    }
  }
  ```

### **Mengirim Konfigurasi Baru**

- **Topik**: `jamur/config/set`
- **Payload**: Sebuah objek JSON yang berisi **hanya kunci yang ingin diubah**.
- **Contoh Kode**:
  ```javascript
  function handleSaveSettings(newSettings) {
    // newSettings adalah objek, misal: { h_crit: 78.5, schedules: [8, 14, 20] }
    if (client && client.connected) {
      const payload = JSON.stringify(newSettings);
      client.publish("jamur/config/set", payload, { qos: 1 });
      showToast("Pengaturan baru sedang dikirim...");
    }
  }
  ```

---

## 4. Alur Kerja Cerdas: Update Pengaturan dengan Konfirmasi

Ini adalah alur terbaik untuk memastikan perubahan konfigurasi berhasil diterapkan.

1.  **Inisialisasi**: Frontend men-_subscribe_ ke topik `jamur/config/get`. Saat terhubung, ia akan langsung menerima konfigurasi terakhir yang disimpan perangkat dan menggunakannya untuk mengisi form pengaturan.
2.  **User Mengubah Data**: Pengguna mengubah nilai di form, misalnya mengubah jadwal.
3.  **Kirim Perubahan**: Saat pengguna menekan "Simpan", frontend:
    - Menampilkan _loading spinner_ dan menonaktifkan tombol simpan.
    - Mengirim objek JSON berisi perubahan ke topik `jamur/config/set`.
4.  **Menunggu Konfirmasi**: Perangkat IoT akan menerima perubahan, menyimpannya ke memori, lalu **mempublikasikan kembali seluruh konfigurasi barunya** ke topik `jamur/config/get`.
5.  **Konfirmasi Diterima**: Handler `on('message')` di frontend akan mendeteksi pesan baru di topik `jamur/config/get`.
    - Perbarui _state_ aplikasi dengan data konfigurasi yang baru diterima ini.
    - Hilangkan _loading spinner_, aktifkan kembali tombol, dan tampilkan notifikasi "Pengaturan berhasil diperbarui!".

Alur ini memastikan bahwa apa yang ditampilkan di frontend selalu sinkron dengan apa yang sebenarnya berjalan di perangkat.
