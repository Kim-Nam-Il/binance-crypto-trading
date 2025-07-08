# 🚀 Binance Crypto Trading Suite

Advanced cryptocurrency trading program collection utilizing Binance API.

## 📁 Project Structure

```
Binance/
├── trade/          # Main trading program
│   ├── src/        # C++ source code
│   ├── include/    # Header files
│   ├── Docker configuration files
│   └── README.md   # Detailed usage guide
└── simulation/     # Trading simulation tools
```

## 🎯 Key Features

### 📊 Trade (Main Trading Program)
- **Spot Trading**: Supports BTC, ETH, BNB, ADA, XRP, SOL, DOT, DOGE, AVAX, MATIC, LTC, LINK
- **Futures Trading**: 12+ cryptocurrencies support, 1x-125x leverage
- **Security**: Encrypted API key storage, master password system
- **Cross-platform**: Docker support (Windows, Linux, macOS)
- **Real-time Monitoring**: Position management, P&L tracking
- **Safety Features**: Automatic LOT_SIZE/NOTIONAL filter handling

### 🔬 Simulation (Simulation Tools)
- Backtesting and strategy testing
- Risk analysis tools
- Virtual trading environment

## 🚀 Quick Start

### 1. Run Trade Program
```bash
cd trade
./docker-run.sh    # Linux/macOS
docker-run.bat     # Windows
```

### 2. Enhanced Security Mode (Recommended)
```bash
cd trade
./docker-run.sh    # Run in enhanced security mode
```

## 📋 Requirements

- Docker & Docker Compose
- Binance API keys (Spot trading and Futures trading permissions)
- Minimum 1GB RAM

## ⚠️ Important Warnings

1. **API Key Security**: Never share your API keys
2. **Futures Trading Risk**: High leverage can cause significant losses
3. **Test Environment**: Test thoroughly in test environment before actual trading
4. **Fund Management**: Only trade with amounts you can afford to lose

## 🔗 Detailed Documentation

For detailed usage of each project, refer to the README.md in respective folders:
- [Trade Program Detailed Guide](trade/README.md)
- [Simulation Tools Guide](simulation/README.md)

## 🤝 Contributing

Issue reports, feature suggestions, and pull requests are welcome!

## 📄 License

MIT License - Feel free to use.

## ⚖️ Disclaimer

This program is provided for educational and research purposes. We are not responsible for any losses from actual trading. Please trade at your own judgment and responsibility.

---

**Made with ❤️ by Kim Nam-Il** 