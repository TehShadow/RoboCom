# 🚀 Lightweight Robotics Middleware Framework (C++17)

A modern, modular, lightweight C++17 middleware framework for building **distributed robotics / IoT systems** — inspired by **ROS2** and **DDS**.

---

## 🎯 Features

✅ Publish / Subscribe API  
✅ Service Client / Server API  
✅ Peer Discovery (multicast)  
✅ Pluggable Transports:
- PeerToPeerTcpTransport
- TcpTransport
- UdpTransport

✅ TypedSubscriber  
✅ TypedServiceClient  
✅ Thread-safe API  
✅ Serialization-ready  
✅ Example apps  
✅ Minimal dependencies (only C++17 + pthread)  

---

## 📚 Architecture

```
+---------------+      +--------------------+
|    Node       | ---> |  TransportManager   |
+---------------+      +--------------------+
       |                        |
       V                        V
  +----------+            +-------------------------+
  | Publisher|            | PeerToPeerTcpTransport  |
  +----------+            | TcpTransport            |
  +-----------+           | UdpTransport            |
  | Subscriber|           +-------------------------+
  +-----------------+
  | TypedSubscriber |
  +--------------------+
  | ServiceClient      |
  | TypedServiceClient |
  +--------------------+
  | ServiceServer   |
  +-----------------+
```

---

## 🔧 Build & Run

### Requirements:

- g++ >= 9.x (C++17)
- Linux (tested Ubuntu 22.04)
- pthread

### Build:

```bash
make all
```

### Run examples:

```bash
# Service example
make run_service_server
make run_service_client

# Pub/Sub example
make run_pub_sub

# UDP Pub/Sub
make run_udp_pub_sub

# TCP Pub/Sub
make run_tcp_pub_sub
```

---

## 🚀 Folder Structure

```
include/               # Public headers
src/                   # Transport implementations
examples/              # Example apps (Pub/Sub, Service)
messages/              # Example message definitions
build/                 # Build output
```

---

## 🌟 Example Apps

- `examples/example_service_client.cpp`  
- `examples/example_service_server.cpp`  
- `examples/example_pub_sub.cpp`  
- `examples/example_udp_pub_sub.cpp`  
- `examples/example_tcp_pub_sub.cpp`

---


## License

MIT License