#include "WindowsBilling.h"
// #include "Trace.h"
// #include "Menus/PicaDialog.h"

#if 0
#include "s3eWindowsStoreBilling.h"

#define IAP_PRODUCT_NAME "FullVersion2"

struct BillingData
{
  BillingData() : mGameSettings(0) {}
  GameSettings* mGameSettings;
};

BillingData gBillingData;

int32_t onAppData(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingCurrentAppData* data = (s3eWindowsStoreBillingCurrentAppData*) systemData;
  TRACE("*** Current App Data ***");
  //TRACE("AppId: %s", data->AppId);
  //TRACE("URI: %s", data->LinkURI);
  TRACE("Trial: %s", data->IsTrial ? "true" : "false");
  TRACE("Active: %s", data->IsActive ? "true" : "false");
  TRACE("ExpirationDate: %s", data->ExpirationDate);
  return 0;
}

int32_t onProdLicense(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingProductLicenseData* data = (s3eWindowsStoreBillingProductLicenseData*) systemData;
  if (data)
  {
    TRACE("*** Product License Data ***");
    TRACE("ProdId: %s", data->ProductId);
    TRACE("Active: %s", data->IsActive ? "true" : "false");
    TRACE("Type: %s", data->IsConsumable ? "consumable" : "durable");

    if (strcmp(IAP_PRODUCT_NAME, data->ProductId) == 0)
      gBillingData.mGameSettings->mStatistics.mIsInTrialMode = !data->IsActive;

    if (!gBillingData.mGameSettings->mStatistics.mIsInTrialMode)
    {
      TRACE("Got license info - not in trial mode");
      gBillingData.mGameSettings->mStatistics.mAppType = Statistics::APP_FULL;
    }
    else
    {
      TRACE("Got license info - in trial mode");
      gBillingData.mGameSettings->mStatistics.mAppType = Statistics::APP_DEMO;
    }
  }
  return 0;
}

int32_t onListing(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingListingInformation* data = (s3eWindowsStoreBillingListingInformation*) systemData;

  if (data)
  {
    TRACE("*** Listing ***");
    TRACE("Name: %s", data->Name);
    TRACE("Description: %s", data->Description);
    TRACE("Price: %s", data->FormattedPrice);
    TRACE("NumProducts: %d", data->NumberOfProducts);
    TRACE("AgeRating: %d", data->AgeRating);
    TRACE("CurrentMarket: %s", data->CurrentMarket);

    for (int i=0; i<data->NumberOfProducts; i++)
    {
      TRACE("Product %d: %s (%s)", i, data->ProductListing[i]->Name, data->ProductListing[i]->ProductId);
    }
  }

  return 0;
}

int32_t onPurchase(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingProductReceiptDataFull* data = (s3eWindowsStoreBillingProductReceiptDataFull*) systemData;
  if (data)
  {
    const char* pid = data->ProductData.ProductId;
    TRACE("*** Receipt ***");
    //TRACE("ReceiptData.CertificateId %s", data->ReceiptData.CertificateId);
    TRACE("ReceiptData.ReceiptDate %s", data->ReceiptData.ReceiptDate);
    TRACE("ReceiptData.ReceiptDeviceId %s", data->ReceiptData.ReceiptDeviceId);
    TRACE("ReceiptData.Version %s", data->ReceiptData.Version);

    //TRACE("ProductData.AppId %s", data->ProductData.AppId);
    TRACE("ProductData.ExpirationDate %s", data->ProductData.ExpirationDate);
    //TRACE("ProductData.Id %s", data->ProductData.Id);
    TRACE("ProductData.ProductId %s", data->ProductData.ProductId);
    TRACE("ProductData.ProductType: %s", data->ProductData.ProductType == S3E_WINDOWSSTOREBILLING_PRODUCTTYPE_CONSUMABLE ? "consumable" : "durable");
    TRACE("ProductData.PurchaseDate %s", data->ProductData.PurchaseDate);

    //TRACE("RawXML: %s", data->RawXML);

    TRACE("PurchasePrice %s", data->PurchasePrice);
    TRACE("PurchaseDate %s", data->PurchaseDate);
    //TRACE("PublisherUserId %s", data->PublisherUserId);
    //TRACE("PublisherDeviceId %s", data->PublisherDeviceId);
    //TRACE("MicrosoftProductId %s", data->MicrosoftProductId);

    if (strcmp(IAP_PRODUCT_NAME, data->ProductId) == 0)
    {
      gBillingData.mGameSettings->mStatistics.mIsInTrialMode = false;
      gBillingData.mGameSettings->mStatistics.mAppType = Statistics::APP_FULL;
      TRACE_FILE_IF(1) TRACE("Saving settings");
      gBillingData.mGameSettings->SaveToFile("Usersettings/Settings.xml");
    }
    const Language language = gBillingData.mGameSettings->mOptions.mLanguage;
    ShowDialog("PicaSim", TXT(PS_PURCHASESUCCESSFUL), TXT(PS_OK));
  }
  return 0;
}

int32_t onApplicationReceipt(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingApplicationReceiptDataFull* data = (s3eWindowsStoreBillingApplicationReceiptDataFull*) systemData;
  if (data)
  {
    TRACE("*** Receipt ***");
    //TRACE("ReceiptData.CertificateId %s", data->ReceiptData.CertificateId);
    TRACE("ReceiptData.ReceiptDate %s", data->ReceiptData.ReceiptDate);
    TRACE("ReceiptData.ReceiptDeviceId %s", data->ReceiptData.ReceiptDeviceId);
    //if (data->ReceiptData.PublisherDeviceId)
    //  TRACE("ReceiptData.PublisherDeviceId %s", data->ReceiptData.PublisherDeviceId);
    TRACE("ReceiptData.Version %s", data->ReceiptData.Version);

    //TRACE("AppData.AppId %s", data->AppData.AppId);
    //TRACE("AppData.Id %s", data->AppData.Id);
    TRACE("AppData.LicenseType %s", data->AppData.LicenseType);
    TRACE("AppData.PurchaseDate %s", data->AppData.PurchaseDate);

    for(unsigned int unProductIndex = 0; unProductIndex < data->unProductsCount; ++unProductIndex)
    {
      s3eWindowsStoreBillingProductReceiptData& prod = data->ProductsData[unProductIndex];
      TRACE("ProductData.ProductId %s", prod.ProductId);
      //TRACE("ProductData.AppId %s", prod.AppId);
      TRACE("ProductData.ExpirationDate %s", prod.ExpirationDate);
      //TRACE("ProductData.Id %s", prod.Id);
      TRACE("ProductData.ProductType: %s", prod.ProductType == S3E_WINDOWSSTOREBILLING_PRODUCTTYPE_CONSUMABLE ? "consumable" : "durable");
      TRACE("ProductData.PurchaseDate %s", prod.PurchaseDate);
    }
    //TRACE("RawXML: %s", data->RawXML);
  }
  return 0;
}

int32_t onReport(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingFulfillmentData* data = (s3eWindowsStoreBillingFulfillmentData*) systemData;
  if (data)
  {
    const char* pid = data->ProductId;
    TRACE("Reported fulfilled: %s", data->ProductId);
  }
  return 0;
}

int32_t onLicense(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingLicenseInformation* info = (s3eWindowsStoreBillingLicenseInformation*) systemData;
  TRACE("*** License ***");
  TRACE("Active: %s", info->IsActive ? "true" : "false");
  TRACE("Trial: %s", info->IsTrial ? "true" : "false");
  TRACE("Expiration date: %s", info->ExpirationDate);
  TRACE("NumLicenses: %d", info->NumberOfProductLicenses);
  for (int i = 0; i<info->NumberOfProductLicenses; i++)
  {
    TRACE("License %d: (%s) %s (%s)",
      i,
      info->ProductLicenses[i]->IsConsumable ? "consumable" : "durable",
      info->ProductLicenses[i]->ProductId,
      info->ProductLicenses[i]->IsActive ? "active" : "not active");
  }
  return 0;
}

int32_t onUnfulfilledConsumable(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  s3eWindowsStoreBillingUnfulfilledConsumableList* data = (s3eWindowsStoreBillingUnfulfilledConsumableList*) systemData;
  TRACE("*** Current Unfulfilled Consumables ***");
  for(unsigned int unUnfulfilledConsumableIndex = 0
    ;unUnfulfilledConsumableIndex < data->NumberOfUnfulfilledConsumables
    ;++unUnfulfilledConsumableIndex
    )
  {
    TRACE("ProductId: %s", data->UnfulfilledConsumables[unUnfulfilledConsumableIndex]->ProductId);
    TRACE("OfferID: %s", data->UnfulfilledConsumables[unUnfulfilledConsumableIndex]->OfferId);
    TRACE("TransactionId: %s", data->UnfulfilledConsumables[unUnfulfilledConsumableIndex]->TransactionId);
  }
  return 0;
}

int32_t onErrorBuyFull(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  const Language language = gBillingData.mGameSettings->mOptions.mLanguage;
  s3eWindowsStoreBillingError data = *(s3eWindowsStoreBillingError*) systemData;
  switch (data)
  {
  case S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_CANCELED:
    TRACE("BUY FULL PRODUCT: Purchase cancelled.");
    ShowDialog("PicaSim", TXT(PS_PURCHASECANCELLED), TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_PURCHASE:
    TRACE("BUY FULL PRODUCT: Error purchase.");
    ShowDialog("PicaSim", TXT(PS_PURCHASEERROR), TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_PURCHASED:
    TRACE("BUY FULL PRODUCT: Product has already been purchased.");
    ShowDialog("PicaSim", TXT(PS_PURCHASEALREADY), TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_NONE:
    TRACE("BUY FULL PRODUCT: Purchase successful.");
    ShowDialog("PicaSim", TXT(PS_PURCHASESUCCESSFUL), TXT(PS_OK));
    break;
  default:
    TRACE("BUY FULL PRODUCT: Error UNKNOWN.");
    break;
  }
  return 0;
}

int32_t onError(void* systemData, void* userData)
{
  TRACE_FUNCTION();
  const Language language = gBillingData.mGameSettings->mOptions.mLanguage;
  s3eWindowsStoreBillingErrorData* data = (s3eWindowsStoreBillingErrorData*) systemData;
  if (data == NULL)
  {
    TRACE("Error but no data.");
    ShowDialog("PicaSim", "Unknown store/billing error", TXT(PS_OK));
    return 1;
  }

  switch (data->code)
  {
  case S3E_WINDOWSSTOREBILLING_ERR_NONE:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_NONE:");
    ShowDialog("PicaSim", "Unknown store/billing error", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_APPDATA:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_APPDATA:");
    ShowDialog("PicaSim", "Error getting app data", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_LISTING:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_LISTING:");
    ShowDialog("PicaSim", "Error getting app listing", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_LICENSE:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_LICENSE:");
    // Happens all the time - is this just because the IAP isn't available?
    //ShowDialog("PicaSim", "Store/billing: License error", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_PURCHASE:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_PURCHASE:");
    ShowDialog("PicaSim", "Error purchasing product", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_PURCHASED:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_PURCHASED:");
    ShowDialog("PicaSim", "Product has already been purchased", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_FULFILMENT:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_FULFILMENT:");
    ShowDialog("PicaSim", "Error fulfilling purchase", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_CANCELED:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_PURCHASE_CANCELED:");
    ShowDialog("PicaSim", "Purchase cancelled", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_FULFILLMENT:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_FULFILLMENT:");
    ShowDialog("PicaSim", "Fulfillment error", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_UNFULFILLEDCONSUMABLES:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_UNFULFILLEDCONSUMABLES:");
    ShowDialog("PicaSim", "Unfulfilled consumables error", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_UNFULFILLEDCONSUMABLES_CANCELED:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_UNFULFILLEDCONSUMABLES_CANCELED:");
    ShowDialog("PicaSim", "Unfulfilled consumables error/cancellation", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_APPRECEIPT:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_APPRECEIPT:");
    ShowDialog("PicaSim", "App receipt error", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_APPRECEIPT_CANCELED:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_APPRECEIPT_CANCELED:");
    ShowDialog("PicaSim", "App receipt cancelled", TXT(PS_OK));
    break;
  case S3E_WINDOWSSTOREBILLING_ERR_UNKNOWN:
    TRACE("Error S3E_WINDOWSSTOREBILLING_ERR_UNKNOWN:");
    ShowDialog("PicaSim", "Unknown store/billing error", TXT(PS_OK));
    break;
  default:
    TRACE("Error unexpected.");
    ShowDialog("PicaSim", "Store/billing: Unexpected error", TXT(PS_OK));
    break;
  }
  TRACE("%s %s", data->Message, data->ProductId == NULL ? "" : data->ProductId);

  return 0;
}



//---------------------------------------------------------------------------------------------------------------------
void InitWindowsBilling(GameSettings& gameSettings)
{
  TRACE_FUNCTION();
  gBillingData.mGameSettings = &gameSettings;

  if (!s3eWindowsStoreBillingAvailable())
  {
    TRACE("Windows Billing is not available");
    return;
  }
  s3eWindowsStoreBillingInit();

  // Register for in-app events:
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_CURRENTAPP, onAppData, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_PRODUCTLICENSE, onProdLicense, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_LISTINGINFORMATION, onListing, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_LICENSEINFORMATION, onLicense, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_PRODUCTPURCHASE, onPurchase, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_FULFILLMENT, onReport, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_UNFULFILLEDCONSUMABLES, onUnfulfilledConsumable, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_APPRECEIPT, onApplicationReceipt, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_ERROR, onError, NULL);
  s3eWindowsStoreBillingRegister(S3E_WINDOWSSTOREBILLING_BUY, onErrorBuyFull, NULL);


#if 0
  {
    // This allows testing purchases multiple times
    const Language language = gBillingData.mGameSettings->mOptions.mLanguage;
    int button = ShowDialog("PicaSim", "Report product fulfillment first?", TXT(PS_NO), TXT(PS_OK));
    if (button == 1)
    {
      TRACE("Calling s3eWindowsStoreBillingReportConsumableFulfillment");
      s3eResult result = s3eWindowsStoreBillingReportConsumableFulfillment(IAP_PRODUCT_NAME);
      if (result != S3E_RESULT_SUCCESS)
      {
        ShowDialog("PicaSim", "Error reporting fulfillment", TXT(PS_OK));
      }
      gBillingData.mGameSettings->mStatistics.mIsInTrialMode = true;
      s3eDeviceYield(10);
      return;
    }
  }
#endif

  // Note - testing on win10 uses trial-mode.xml
}

//---------------------------------------------------------------------------------------------------------------------
void TerminateWindowsBilling()
{
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsCheckForTrialVersion()
{
  TRACE_FUNCTION();
  if (!gBillingData.mGameSettings)
    return;
  const Language language = gBillingData.mGameSettings->mOptions.mLanguage;
  if (gBillingData.mGameSettings->mOptions.mFrameworkSettings.mOS != S3E_OS_ID_WIN10)
  {
    return;
  }
  if (!s3eWindowsStoreBillingAvailable())
  {
    return;
  }
  TRACE("Calling s3eWindowsStoreBillingGetLicenseInformation");
  s3eResult result = s3eWindowsStoreBillingGetLicenseInformation(IAP_PRODUCT_NAME);
  if (result != S3E_RESULT_SUCCESS)
  {
    ShowDialog("PicaSim", "Error checking for license information", TXT(PS_OK));
  }
}

//---------------------------------------------------------------------------------------------------------------------
void WindowsBuyFullVersion()
{
  TRACE_FUNCTION();
  if (!gBillingData.mGameSettings)
    return;

  const Language language = gBillingData.mGameSettings->mOptions.mLanguage;
  if (gBillingData.mGameSettings->mOptions.mFrameworkSettings.mOS != S3E_OS_ID_WIN10)
  {
    ShowDialog("PicaSim", "Buy full version: Unexpected platform", TXT(PS_OK));
    return;
  }
  if (!s3eWindowsStoreBillingAvailable())
  {
    ShowDialog("PicaSim", "Buy full version: Windows billing is not available", TXT(PS_OK));
    return;
  }

  int button = ShowDialog("PicaSim", TXT(PS_PURCHASEFROMSTORE), TXT(PS_CANCEL), TXT(PS_OK));
  if (button == 0)
    return;
  TRACE("Calling s3eWindowsStoreBillingRequestProductPurchase %s", IAP_PRODUCT_NAME);
  s3eResult result = s3eWindowsStoreBillingRequestProductPurchase(IAP_PRODUCT_NAME);
  if (result != S3E_RESULT_SUCCESS)
  {
    ShowDialog("PicaSim", TXT(PS_PURCHASEERROR), TXT(PS_OK));
  }
}

#else

void InitWindowsBilling(GameSettings& gameSettings) {}
void TerminateWindowsBilling() {}
void WindowsCheckForTrialVersion() {}
void WindowsBuyFullVersion() {}

#endif