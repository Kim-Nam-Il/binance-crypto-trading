# Binance Bitcoin Minimum Quantity Trading Program (Security Enhanced)

This is a security-enhanced C++ trading program that can buy and sell minimum quantities of Bitcoin using the Binance API.

## 🚀 Quick Start

**Windows Users:**
```batch
docker-run.bat
```

**Linux/macOS Users:**
```bash
./docker-run.sh
```

The program will automatically build and run!

## Key Features

### Spot Trading Features
- Account balance inquiry (BTC, USDT)
- Current price inquiry for various cryptocurrencies
- Buy/sell cryptocurrencies with minimum quantity
- Buy/sell cryptocurrencies with custom quantity
- Real-time order execution and result confirmation

**Supported Assets (Spot Trading):**
- BTC (Bitcoin) 
- ETH (Ethereum)
- BNB (Binance Coin)
- ADA (Cardano)
- XRP (Ripple)
- SOL (Solana)
- DOT (Polkadot)
- DOGE (Dogecoin)
- AVAX (Avalanche)
- MATIC (Polygon)
- LTC (Litecoin)
- LINK (Chainlink)

*Note: Currently, actual buying/selling is only supported for BTC, while other assets support price inquiry only.*

### Futures Trading Features 🚀
- **Futures Account Information**: Check margin balance, unrealized P&L, available balance
- **Position Management**: View current positions, real-time profit/loss tracking
- **Leverage Setting**: Freely set 1x~125x leverage
- **Long/Short Positions**: Enter long/short positions with market orders
- **Position Closing**: Close positions completely with one click
- **Limit Orders**: Execute limit orders at desired prices
- **Risk Management**: Real-time unrealized P&L monitoring

**Supported Assets (Futures Trading):**
- BTC (Bitcoin)
- ETH (Ethereum)
- BNB (Binance Coin)
- ADA (Cardano)
- XRP (Ripple)
- SOL (Solana)
- DOT (Polkadot)
- DOGE (Dogecoin)
- AVAX (Avalanche)
- MATIC (Polygon)
- LTC (Litecoin)
- LINK (Chainlink)

*All futures trading features are fully supported for the above assets.*

### 🔍 Futures Trading Symbol List Query Feature (Menu 18)
- **Real-time List**: Query actually tradable symbols from Binance API in real-time
- **Popular Coins First**: Show popular coins like BTC, ETH, BNB first
- **Alphabetical Sorting**: Sort entire list alphabetically for easy searching
- **Detailed Information**: Display minimum order quantity, minimum order amount, trading status for each symbol
- **100+ Symbols Support**: Support 100+ USDT pairs from Bitcoin to altcoins

This feature allows you to check which cryptocurrencies are available for futures trading in advance!

### Security Features 🔒
- **Encrypted API Key Storage**: API keys and secret keys are encrypted and stored safely
- **Master Password System**: Protect all keys with one master password
- **24-hour Session Management**: Valid session maintained for 24 hours after one authentication
- **Automatic Session Expiration**: Session automatically expires after 24 hours, requiring re-authentication
- **Secure Memory Management**: Safely remove sensitive data from memory
- **Hidden Password Input**: Passwords are not displayed on screen during input
- **Key Management Features**: Delete stored keys, change master password, etc.

### Docker Support 🐳
- **Cross-platform**: Works identically on Mac, Linux, Windows
- **Automatic Dependency Resolution**: All necessary libraries included in container
- **Easy Execution**: Automated from build to execution with one script
- **Dual Security Mode**: Choose between convenience and enhanced security
  - **Normal Mode**: Data persistence (host storage)
  - **Enhanced Security Mode**: Container isolation (internal storage)

## Required Dependencies

### macOS (Using Homebrew)
```bash
brew install curl openssl pkg-config cmake
```

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev libssl-dev pkg-config cmake build-essential
```

## Compilation and Execution

### Method 1: Using Docker (Recommended) 🐳

Using Docker works identically on Mac, Linux, and Windows environments.

1. **Install Docker and Docker Compose**
   - Install [Docker Desktop](https://www.docker.com/products/docker-desktop/)

2. **Build and Run Project**

**Simple Execution (Recommended):**
```bash
# Run in enhanced security mode
# Linux/macOS
./docker-run.sh
# Windows
docker-run.bat
```

**Manual Execution:**
```bash
# Enhanced security mode
docker-compose -f docker-compose-secure.yml up --build
# or
docker build -f Dockerfile.secure -t binance-trader-secure .
docker run -it --rm --security-opt no-new-privileges:true --tmpfs /tmp:rw,noexec,nosuid,size=100m binance-trader-secure
```

3. **Enhanced Security Mode**
   - Encrypted key files are stored only inside the container
   - Data is completely deleted when container is removed
   - Strong security, inaccessible from host
   - Passwords are not displayed on screen but input normally

4. **Tested Environments**
   - ✅ **Windows 10/11**: Using Docker Desktop
   - ✅ **Ubuntu 20.04**: Build and run inside Docker container
   - ✅ **Automatic Dependency Installation**: libcurl, OpenSSL, CMake, build-essential
   - ✅ **Code Compatibility**: C++ 17 standard compliance

### Method 2: Direct Compilation

1. Build Project
```bash
mkdir build
cd build
cmake ..
make
```

2. Run Program
```bash
./binance_trader
```

## Binance API Key Setup

1. Login to [Binance](https://www.binance.com)
2. Go to API Management
3. Create new API key
4. **Permission Settings**:
   - **Spot Trading Only**: Enable only Spot Trading permission
   - **Including Futures Trading**: Enable Spot Trading + Futures Trading permissions 🚀
   - **Recommended**: Set IP restrictions for enhanced security
5. Keep API key and secret key safe

### Additional Futures Trading Setup ⚠️
- First activate Binance futures trading account
- Deposit USDT to futures trading account (separate from spot account)
- Default leverage is set to safe 1x (can be manually changed if needed)
- Futures trading involves high risk, use only after sufficient learning

## Usage

### First Run
1. Run program
2. Set new master password (enter twice for confirmation)
3. Enter Binance API key and secret key
4. Keys are encrypted and stored safely

### Subsequent Runs
1. Run program
2. Enter master password (valid for 24 hours)
3. Automatically load encrypted keys

### Menu Options

**Spot Trading Features:**
1. **Account Information Query**: Check current BTC and USDT balance
2. **Cryptocurrency Current Price Query**: Query current market price of selected cryptocurrency
3. **Buy Cryptocurrency Minimum Quantity**: Buy cryptocurrency with minimum quantity allowed by exchange
4. **Sell Cryptocurrency Minimum Quantity**: Sell cryptocurrency with minimum quantity allowed by exchange
5. **Buy with Custom Quantity**: Buy cryptocurrency with desired quantity
6. **Sell with Custom Quantity**: Sell cryptocurrency with desired quantity

**Futures Trading Features:**
11. **Futures Account Information Query**: Check margin balance, unrealized P&L, available balance
12. **Futures Position Query**: Check current position status and profit rate for selected cryptocurrency
13. **Leverage Setting**: Set 1x~125x leverage for selected cryptocurrency (default: 1x safe mode)
14. **Enter Long Position**: Enter market long position for selected cryptocurrency (bet on price increase)
15. **Enter Short Position**: Enter market short position for selected cryptocurrency (bet on price decrease)
16. **Close Position**: Immediately close current position for selected cryptocurrency to realize P&L
17. **Futures Limit Order**: Execute limit order at desired price for selected cryptocurrency
18. **Query Futures Trading Symbol List**: Query all actually tradable USDT pairs on Binance 🆕

**System Features:**
7. **Session Status Check**: Check current session validity and expiration time
8. **Order Permission Test**: Test API key trading permissions without actual trading 🔧
9. **Delete Stored Keys**: Completely delete encrypted key files
10. **Change Master Password**: Change existing password to new password

## Security Warnings

### Enhanced Security Features
- ✅ **Encrypted Storage**: API keys are encrypted and stored locally
- ✅ **Master Password**: Protect all keys with one strong password
- ✅ **Session Management**: Re-authentication required after 24 hours
- ✅ **Memory Security**: Sensitive data is safely removed from memory

### Additional Security Recommendations
- **Never** hardcode API keys and secret keys in code
- Grant only minimum permissions to API keys (Spot Trading only)
- Set IP restrictions for enhanced security
- Set master password strongly (8+ characters, include special characters)
- Recommend testing on testnet first
- Change master password regularly

## Minimum Order Quantity

- Minimum order quantity for BTCUSDT pair is typically 0.00001 BTC
- Program automatically queries and displays minimum order quantity

## Warnings

### General Warnings
- This program is written for educational and testing purposes
- Test thoroughly before actual trading
- Slippage may occur as market orders are used
- Trading fees apply

### Special Futures Trading Warnings ⚠️
- **High Risk**: Futures trading can cause large losses due to leverage
- **Fund Management**: Use only a portion of investable amount
- **Leverage Caution**: High leverage involves risk of large losses along with large profits
- **Forced Liquidation**: Positions may be forcibly liquidated if losses increase
- **24-hour Trading**: Futures markets fluctuate 24 hours, requiring continuous monitoring
- **Education Required**: Do not use without sufficient understanding of futures trading
- **Testnet Recommended**: Practice on Binance testnet before actual trading

## Troubleshooting

### Interactive Input Issues (Password Input) 🔑

#### Problem: No response when entering password
```
First time user! Please set a new master password.
Enter new master password: [Seems like input doesn't work here]
```

**Cause:** Interactive input in Docker container may not work properly in Windows PowerShell

**Solutions:**

1. **Use Interactive-only Script (Recommended):**
   ```bash
   # Windows
   docker-run-secure-interactive.bat
   
   # Linux/macOS
   ./docker-run-secure-interactive.sh
   ```

2. **Use Direct Docker Command:**
   ```bash
   docker run -it --rm --name binance-trader-secure \
     --security-opt no-new-privileges:true \
     --tmpfs /tmp:rw,noexec,nosuid,size=100m \
     binance-trader-secure
   ```

3. **Use Windows Terminal:**
   - Use Windows Terminal instead of PowerShell
   - Provides better interactive support

**⚠️ Important Notes:**
- 🔐 **Password input is not displayed on screen but is being entered**
- ⌨️ **Type slowly and accurately, then press Enter**
- 🔄 **Password confirmation must be entered identically**
- 💡 **Cursor may not move during input but this is normal**

### Docker-related Issues

#### Container Name Conflict Error
```
docker: Error response from daemon: Conflict. The container name "/binance-trader-secure" is already in use by container "...". You have to remove (or rename) that container to be able to reuse that name.
```

**Cause:** Previous container remains, preventing creation of new container with same name

**Solutions:**

1. **Delete existing container and run new one (Recommended):**
   ```bash
   # Force delete existing container
   docker rm -f binance-trader-secure
   
   # Run again
   ./docker-run-secure-interactive.bat
   ```

2. **Connect to existing running container:**
   ```bash
   # Connect to running container
   docker exec -it binance-trader-secure /bin/bash
   
   # Or run program directly
   docker exec -it binance-trader-secure ./build/binance_trader
   ```

3. **Check existing container status:**
   ```bash
   # Check container status
   docker ps -a | findstr binance-trader-secure
   
   # If running: STATUS shows "Up"
   # If stopped: STATUS shows "Exited"
   ```

4. **Restart stopped container:**
   ```bash
   # Start stopped container
   docker start binance-trader-secure
   
   # Connect after start
   docker exec -it binance-trader-secure ./build/binance_trader
   ```

5. **Complete cleanup and fresh start (Recommended):**
   ```bash
   # Delete all related containers
   docker rm -f binance-trader-secure
   
   # Also delete image if desired
   docker rmi binance-trader-secure
   
   # Run fresh
   ./docker-run-secure-interactive.bat
   ```

**💡 Simplest Solution:**
```bash
docker rm -f binance-trader-secure && ./docker-run-secure-interactive.bat
```

#### Docker Desktop Execution Error
```
error during connect: Get "http://%2F%2F.%2Fpipe%2FdockerDesktopLinuxEngine/v1.48/..."
```
**Solution:**
1. Check if Docker Desktop is running
2. Run "Docker Desktop" from Windows Start menu
3. Check Docker whale icon in system tray
4. Wait 1-2 minutes for Docker Desktop to fully start

#### Docker Compose Version Warning
```
the attribute `version` is obsolete
```
**Solution:** Already fixed. Version attribute is unnecessary in latest Docker Compose.

### Compilation Errors

#### C++ Reserved Word Conflict
```
error: 'signed' specified with 'bool'
```
**Solution:** Already fixed. Changed `signed` parameter to `is_signed`.

#### OpenSSL Header Error
```
fatal error: openssl/pbkdf2.h: No such file or directory
```
**Solution:** Already fixed. Removed unused headers.

### Direct Compilation Issues
- Check if OpenSSL and libcurl are properly installed
- Check if pkg-config can find libraries
- Check Visual Studio Build Tools installation (Windows)

### API Errors and Trading Failures

#### Problem: Buy/sell orders not executing
```
Do you really want to buy? (y/N): y
Executing buy order...
Order failed: [Error message]
```

**Common Causes and Solutions:**

1. **Insufficient API Key Permissions**
   - Check API key settings in Binance
   - Verify **Spot Trading** permission is enabled
   - Check if **Enable Trading** option is checked

2. **IP Restriction Settings**
   - Check IP restrictions in Binance API management
   - Verify current IP address is in allowed list
   - Or disable IP restrictions (not recommended for security)

3. **Insufficient Balance**
   - Check if USDT balance is sufficient for purchase amount
   - Minimum purchase amount: approximately $1-2 (varies with current BTC price)

4. **Below Minimum Order Quantity**
   - Minimum order quantity: 0.00001000 BTC
   - Check minimum order amount based on current BTC price

5. **Network Connection Issues**
   - Check internet connection status
   - Check if firewall blocks Binance API access

6. **Market Conditions**
   - Check if not during trading suspension time
   - Check if not during Binance server maintenance

**Debugging Methods:**

1. **Use Order Permission Test (Menu 8)**: 🔧
   ```
   === Order Permission Test ===
   Test order permissions without actual trading.
   
   Testing buy permission...
   ✅ Buy permission: Normal
   Testing sell permission...
   ✅ Sell permission: Normal
   
   🎉 All order permissions are normal! Actual trading is possible.
   ```

2. **Check API Response**:
   - Check "API Response" messages during program execution
   - Check specific error codes and messages
   - Look up error code meanings in Binance API documentation

3. **Detailed Permission Check**:
   - Trading permission status automatically checked at program start
   - Check Binance settings when "❌ Trading Permission: Disabled" message appears

**Additional Checks:**
- Verify API key and secret key are correct
- Check if account has sufficient balance
- Verify it's actual account, not testnet

### Network Errors
- Check internet connection status
- Check firewall settings
- Check Binance API server status

## File Storage Locations

### Encrypted Key Files

**Docker Normal Mode:**
- Host system: `./data/keys.enc`
- Inside container: `/app/data/keys.enc`

**Docker Enhanced Security Mode:**
- Inside container only: `/app/secure_data/keys.enc`
- Inaccessible from host

**Direct Execution:**
- **Windows**: `%USERPROFILE%\.binance_trader\keys.enc`
- **Linux/macOS**: `~/.binance_trader/keys.enc`

This file is encrypted with master password and cannot be decrypted without the password.

## Additional Docker Commands

### Container Management

**Normal Mode:**
```bash
# Run in background
docker-compose up -d

# Connect to running container
docker-compose exec binance-trader /bin/bash

# Stop container
docker-compose down

# Delete container and images
docker-compose down --rmi all
```

**Enhanced Security Mode:**
```bash
# Run in background
docker-compose -f docker-compose-secure.yml up -d

# Connect to running container
docker-compose -f docker-compose-secure.yml exec binance-trader-secure /bin/bash

# Stop container
docker-compose -f docker-compose-secure.yml down

# Delete container and images
docker-compose -f docker-compose-secure.yml down --rmi all
```

### Log Checking

**Normal Mode:**
```bash
# Check real-time logs
docker-compose logs -f

# Check specific container logs
docker logs binance-trader
```

**Enhanced Security Mode:**
```bash
# Check real-time logs
docker-compose -f docker-compose-secure.yml logs -f

# Check specific container logs
docker logs binance-trader-secure
```

## Build Success Confirmation

When program builds successfully, following message is displayed:

```
✅ Build successful!
🚀 Program started...

=== Binance Bitcoin Minimum Quantity Trading Program (Security Enhanced) ===

First time user! Please set a new master password.
```

### Resolved Major Issues
- ✅ **C++ Reserved Word Conflict**: Fixed `signed` → `is_signed`
- ✅ **OpenSSL Header Compatibility**: Removed unnecessary `pbkdf2.h` header
- ✅ **Docker Compose Version Warning**: Updated to latest format
- ✅ **Cross-platform Build**: Stable build in Ubuntu 20.04 container
- ✅ **Interactive Input Issues**: Improved password input in enhanced security mode
- ✅ **Trading Failure Debugging**: Added API response output and permission check features
- ✅ **Quantity Formatting**: Prevented order errors with accurate 8-decimal formatting
- ✅ **Order Permission Test**: Pre-check API key permissions without actual trading

## Security Mode Comparison

### Normal Mode vs Enhanced Security Mode

| Item | Normal Mode | Enhanced Security Mode |
|------|-------------|------------------------|
| **Data Storage** | Host `./data/` | Container internal only |
| **Data Persistence** | ✅ Maintained after container deletion | ❌ Deleted with container |
| **Host Access** | ⚠️ Files accessible from host | ✅ Inaccessible from host |
| **Container Isolation** | 🔒 Basic isolation | 🔐 Enhanced isolation |
| **Permission Restriction** | Default permissions | Minimum permissions (unprivileged user) |
| **File System** | Read/write | Read-only (except data directory) |
| **Security Options** | Default settings | `--security-opt`, `--cap-drop` applied |
| **Backup Convenience** | ✅ Easy | ⚠️ Only possible while container running |

### Enhanced Security Mode Features

**Applied Security Features:**
- 🔐 **Unprivileged User Execution**: Run without root privileges
- 🔒 **Read-only File System**: Prevent system file changes
- 🛡️ **Permission Restrictions**: Remove unnecessary system permissions
- 🔑 **Container Isolation**: Complete separation from host system
- 🚫 **Prevent New Privilege Acquisition**: `no-new-privileges` applied

**Recommended Usage Scenarios:**
- 🏢 **Production Environment**: Maximum security needed for actual trading
- 🔐 **Sensitive Environment**: High security requirements
- 🧪 **One-time Testing**: Complete deletion needed after testing
- 🌐 **Shared Systems**: When sharing system with other users

## License

This project is provided for educational purposes. Use at your own responsibility. 