#include <esp_log.h>
#include <clwe/color_kem.hpp>

static const char *TAG = "ColorKEM_Test";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting ColorKEM basic operations test");

    try {
        // Create ColorKEM instance with ML-KEM-768 parameters
        clwe::CLWEParameters params(768);
        clwe::ColorKEM kem(params);

        ESP_LOGI(TAG, "ColorKEM instance created with security level %d", params.security_level);

        // Generate key pair
        ESP_LOGI(TAG, "Generating key pair...");
        auto [pk, sk] = kem.keygen();
        ESP_LOGI(TAG, "Key pair generated successfully");

        // Encapsulate shared secret
        ESP_LOGI(TAG, "Encapsulating shared secret...");
        auto [ct, ss] = kem.encapsulate(pk);
        ESP_LOGI(TAG, "Shared secret encapsulated");

        // Decapsulate shared secret
        ESP_LOGI(TAG, "Decapsulating shared secret...");
        clwe::ColorValue recovered_ss = kem.decapsulate(pk, sk, ct);
        ESP_LOGI(TAG, "Shared secret decapsulated");

        // Verify shared secrets match
        if (ss == recovered_ss) {
            ESP_LOGI(TAG, "SUCCESS: Shared secrets match - ColorKEM operations working correctly");
        } else {
            ESP_LOGE(TAG, "FAILURE: Shared secrets do not match");
        }

        ESP_LOGI(TAG, "ColorKEM test completed");

    } catch (const std::exception& e) {
        ESP_LOGE(TAG, "Exception occurred: %s", e.what());
    }
}