# Smart Parking System

A complete full-stack solution for managing parking spaces with real-time availability tracking, user authentication, and reservation management.

## 🎯 Features

### User Features
- User registration and authentication with JWT
- Browse available parking spots
- Advanced filtering (floor, location, vehicle type)
- Real-time spot booking
- Reservation management
- Automatic cost calculation
- View booking history

### Admin Features
- View all parking spots
- Spot availability management
- User management
- Revenue tracking

### Technical Features
- RESTful API backend
- MongoDB database
- JWT authentication
- Responsive React frontend
- Real-time availability updates

## 📁 Project Structure

```
Smart Parking System/
├── backend/                 # Node.js Express API
│   ├── src/
│   │   ├── models/         # MongoDB schemas
│   │   ├── controllers/    # Business logic
│   │   ├── routes/         # API endpoints
│   │   ├── middleware/     # Authentication & validation
│   │   ├── config/         # Database configuration
│   │   └── index.js        # Main server file
│   ├── package.json
│   └── README.md
│
├── frontend/               # React application
│   ├── src/
│   │   ├── components/     # Reusable UI components
│   │   ├── pages/          # Page components
│   │   ├── services/       # API service layer
│   │   ├── App.js          # Main app component
│   │   └── index.js        # Entry point
│   ├── package.json
│   └── README.md
│
└── README.md              # This file
```

## Arduino ESP32 Hardware

The file `parking system.ino` monitors two parking slots with an ESP32 and sends
their status to the backend over Wi-Fi. Before uploading, edit the `ssid`,
`password`, and `serverName` values in the sketch. Use the computer's LAN IP in
`serverName`; `localhost` will point to the ESP32 itself and will not reach the
backend.

### Wiring

| Component | Pin or wire | ESP32 connection |
| --- | --- | --- |
| Slot 1 ultrasonic sensor | TRIG | GPIO 23 |
| Slot 1 ultrasonic sensor | ECHO | GPIO 19 |
| Slot 2 ultrasonic sensor | TRIG | GPIO 25 |
| Slot 2 ultrasonic sensor | ECHO | GPIO 33 |
| Slot 1 green LED | Anode through resistor | GPIO 5 |
| Slot 1 red LED | Anode through resistor | GPIO 18 |
| Slot 2 green LED | Anode through resistor | GPIO 14 |
| Slot 2 red LED | Anode through resistor | GPIO 12 |
| Buzzer | Positive | GPIO 4 |
| SSD1306 OLED | SDA | GPIO 21 |
| SSD1306 OLED | SCL | GPIO 22 |
| All components | GND | ESP32 GND |
| Sensors, LEDs, buzzer, OLED | VCC | Suitable module voltage and common ESP32 ground |

Use a resistor for each LED and verify the voltage requirements of every
module. Many HC-SR04 sensors output a 5 V ECHO signal; protect the ESP32 ECHO
pins with a voltage divider or a 3.3 V-compatible ultrasonic sensor.

### Arduino IDE Setup

1. Select an ESP32 board and the correct serial port.
2. Install `Adafruit GFX Library` and `Adafruit SSD1306` from the Library Manager.
3. Set the Wi-Fi credentials and backend URL in `parking system.ino`.
4. Start the backend on the same local network. The default endpoint is
   `POST /api/parking/hardware-update` on port `5000`.
5. Upload the sketch and open Serial Monitor at `115200` baud.

The OLED and Serial Monitor show whether each slot is `OPEN` or `FULL`. A slot
is treated as occupied when its measured distance is 10 cm or less. Every five
seconds the ESP32 sends JSON similar to:

```json
{
  "slot1": false,
  "slot2": true,
  "recommendedSlot": "SLOT 1"
}
```

## 🚀 Getting Started

### Prerequisites
- Node.js (v14 or higher)
- MongoDB (local or cloud instance)
- npm or yarn

### Backend Setup

1. Navigate to the backend directory:
```bash
cd backend
```

2. Install dependencies:
```bash
npm install
```

3. Create a `.env` file (copy from `.env.example`):
```bash
cp .env.example .env
```

4. Update `.env` with your MongoDB URI and JWT secret:
```
PORT=5000
MONGODB_URI=mongodb://localhost:27017/smart-parking
JWT_SECRET=your_secret_key_here
NODE_ENV=development
```

5. Start MongoDB (if running locally):
```bash
mongod
```

6. Run the server:
```bash
npm run dev
```

The backend will be available at `http://localhost:5000`

### Frontend Setup

1. In a new terminal, navigate to the frontend directory:
```bash
cd frontend
```

2. Install dependencies:
```bash
npm install
```

3. Start the development server:
```bash
npm start
```

The frontend will automatically open at `http://localhost:3000`

## 🔌 API Endpoints

### Authentication
- `POST /api/auth/register` - Register new user
- `POST /api/auth/login` - User login

### Parking
- `GET /api/parking/available` - Get available spots (with filters)
- `GET /api/parking/all` - Get all spots (admin only)
- `POST /api/parking/book` - Book a parking spot
- `POST /api/parking/release` - Release a parking spot
- `GET /api/parking/reservations` - Get user reservations

## 📊 Database Models

### User
- name
- email (unique)
- password (hashed)
- phone
- vehicleNumber (unique)
- role (user/admin)
- isActive
- timestamps

### ParkingSpot
- spotNumber (unique)
- floor
- location (A/B/C/D)
- isAvailable
- vehicleType (compact/standard/large/motorcycle)
- occupiedBy (reference to Reservation)
- timestamps

### Reservation
- user (reference to User)
- parkingSpot (reference to ParkingSpot)
- entryTime
- exitTime
- status (active/completed/cancelled)
- totalCost
- paymentStatus (pending/paid/failed)
- timestamps

## 🔐 Authentication

The system uses JWT (JSON Web Tokens) for authentication:

1. Users register/login to receive a JWT token
2. Token is stored in localStorage
3. Token is included in all authenticated requests
4. Backend validates token on protected routes

## 💻 Development

### Backend Stack
- Express.js - Web framework
- MongoDB - Database
- Mongoose - ODM
- bcryptjs - Password hashing
- jsonwebtoken - JWT handling

### Frontend Stack
- React 18 - UI library
- React Router v6 - Navigation
- Axios - HTTP client
- CSS3 - Styling

## 📝 Sample Test Data

### Default User
```json
{
  "email": "user@test.com",
  "password": "password123",
  "name": "Test User",
  "vehicleNumber": "ABC123"
}
```

### Create Sample Spots (MongoDB)
```javascript
db.parkingspots.insertMany([
  { spotNumber: "A-101", floor: 1, location: "A", isAvailable: true, vehicleType: "standard" },
  { spotNumber: "A-102", floor: 1, location: "A", isAvailable: true, vehicleType: "compact" },
  { spotNumber: "B-201", floor: 2, location: "B", isAvailable: true, vehicleType: "large" },
])
```

## 🛠 Troubleshooting

### MongoDB Connection Error
- Ensure MongoDB is running
- Check the MONGODB_URI in .env

### Port Already in Use
- Backend: Change PORT in .env
- Frontend: The app will prompt to use a different port

### CORS Issues
- Ensure backend is running on port 5000
- Check proxy setting in frontend/package.json

### Token Expiry
- Tokens expire in 24 hours
- User will need to login again

## 🚦 Future Enhancements

- Payment gateway integration
- Email notifications
- SMS alerts
- Mobile app (React Native)
- Real-time WebSocket updates
- Advanced analytics dashboard
- QR code for spot entry
- IoT sensor integration
- Multi-location support
- Subscription plans

## 📄 License

This project is open source and available under the MIT License.

## 👨‍💻 Support

For issues or questions, please check the README files in the backend and frontend directories for more specific information.

---

**Happy Parking! 🚗**
