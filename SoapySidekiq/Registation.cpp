//  Copyright [2018] <Alexander Hurd>"

#include <unistd.h>
#include "SoapySidekiq.hpp"
#include <SoapySDR/Registry.hpp>

static std::vector<SoapySDR::Kwargs> findSidekiq(const SoapySDR::Kwargs &args) {
  std::vector<SoapySDR::Kwargs> results;

  uint8_t number_of_cards = 0;
  uint8_t card_list[SKIQ_MAX_NUM_CARDS];
  char *serial_str;
  pid_t card_owner;
  skiq_xport_type_t type = skiq_xport_type_auto;

  /* query the list of all Sidekiq cards on the PCIe interface */
  if(skiq_get_cards(type, &number_of_cards, card_list) != 0){
    SoapySDR_log(SOAPY_SDR_ERROR,  "Failure: skiq_get_cards");
  }

  for (int i = 0; i < number_of_cards; i++) {
    SoapySDR::Kwargs devInfo;
    bool deviceAvailable = false;

    /* determine the serial number based on the card number */
    if(skiq_read_serial_string(i, &serial_str) != 0){
      SoapySDR_log(SOAPY_SDR_ERROR,  "Failure: skiq_read_serial_string");
    }

    /* get card availability */
    skiq_is_card_avail(i, &card_owner);
    deviceAvailable = (card_owner == getpid());  //  owner must be this process(pid)
    if (!deviceAvailable) {
      SoapySDR_logf(SOAPY_SDR_WARNING, "Unable to access card #%d, owner pid (%d)", i, card_owner);
    }

    std::string deviceLabel = "Epiq Solutions - Sidekiq :: " + std::string(serial_str);

    devInfo["card"] = std::to_string(i);
    devInfo["label"] = deviceLabel;
    devInfo["available"] = deviceAvailable ? "Yes" : "No";
    devInfo["product"] = "Sidekiq";
    devInfo["serial"] = std::string(serial_str);
    devInfo["manufacturer"] = "Epiq Solutions";
    SoapySidekiq::sidekiq_devices.push_back(devInfo);
  }

  //  filtering
  for (int i = 0; i < number_of_cards; i++) {
    SoapySDR::Kwargs devInfo = SoapySidekiq::sidekiq_devices[i];
    if (args.count("card") != 0) {
      if (args.at("card") != devInfo.at("card")) {
        continue;
      }
      SoapySDR_logf(SOAPY_SDR_DEBUG, "Found device by card %s", devInfo.at("card").c_str());
    } else if (args.count("serial") != 0) {
      if (devInfo.at("serial") != args.at("serial")) {
        continue;
      }
      SoapySDR_logf(SOAPY_SDR_DEBUG, "Found device by serial %s", args.at("serial").c_str());
    }
    results.push_back(SoapySidekiq::sidekiq_devices[i]);
  }

  return results;
}

static SoapySDR::Device *makeSidekiq(const SoapySDR::Kwargs &args) {
  return new SoapySidekiq(args);
}

static SoapySDR::Registry registerSidekiq("sidekiq", &findSidekiq, &makeSidekiq, SOAPY_SDR_ABI_VERSION);
