int main() {
    NetworkMetricsLogger logger;

    // Register a callback if needed
    logger.RegisterCallback([](const std::string& libraryName) {
        std::cout << "Callback received for library: " << libraryName << std::endl;
        // Add callback logic here
    });

    // Start logging for RakNet
    logger.StartLogging(5, "RakNet");

    return 0;
}