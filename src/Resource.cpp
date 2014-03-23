#include "Resource.h"
#include "Building.h"
#include "Terrain.h"
#include "Data/Building.h"
#include "Data/CityInfo.h"
#include "Data/Constants.h"
#include "Data/Scenario.h"

void Resource_calculateWarehouseStocks()
{
	for (int i = 0; i < 16; i++) {
		Data_CityInfo.resourceSpaceInWarehouses[i] = 0;
		Data_CityInfo.resourceStored[i] = 0;
	}
	for (int i = 1; i < MAX_BUILDINGS; i++) {
		if (Data_Buildings[i].inUse == 1 && Data_Buildings[i].type == Building_Warehouse) {
			Data_Buildings[i].hasRoadAccess = 0;
			if (Terrain_hasRoadAccess(Data_Buildings[i].x, Data_Buildings[i].y,
					Data_Buildings[i].size, 0, 0)) {
				Data_Buildings[i].hasRoadAccess = 1;
			} else if (Terrain_hasRoadAccess(Data_Buildings[i].x, Data_Buildings[i].y, 3, 0, 0)) {
				Data_Buildings[i].hasRoadAccess = 2;
			}
		}
	}
	for (int i = 0; i < MAX_BUILDINGS; i++) {
		if (Data_Buildings[i].inUse != 1 || Data_Buildings[i].type != Building_WarehouseSpace) {
			continue;
		}
		int warehouseId = Building_getMainBuildingId(i);
		if (Data_Buildings[warehouseId].hasRoadAccess) {
			Data_Buildings[i].hasRoadAccess = Data_Buildings[warehouseId].hasRoadAccess;
			if (Data_Buildings[i].subtype.warehouseResourceId) {
				int loads = Data_Buildings[i].loadsStored;
				int resource = Data_Buildings[i].subtype.warehouseResourceId;
				Data_CityInfo.resourceStored[resource] += loads;
				Data_CityInfo.resourceSpaceInWarehouses[resource] += 4 - loads;
			} else {
				Data_CityInfo.resourceSpaceInWarehouses[Resource_None] += 4;
			}
		}
	}
}

void Resource_calculateWorkshopStocks()
{
	for (int i = 0; i < 6; i++) {
		Data_CityInfo.resourceWorkshopRawMaterialStored[i] = 0;
		Data_CityInfo.resourceWorkshopRawMaterialSpace[i] = 0;
	}
	for (int i = 1; i < MAX_BUILDINGS; i++) {
		if (Data_Buildings[i].inUse != 1 || !BuildingIsWorkshop(Data_Buildings[i].type)) {
			continue;
		}
		Data_Buildings[i].hasRoadAccess = 0;
		if (Terrain_hasRoadAccess(Data_Buildings[i].x, Data_Buildings[i].y, Data_Buildings[i].size, 0, 0)) {
			Data_Buildings[i].hasRoadAccess = 1;
			int room = 2 - Data_Buildings[i].loadsStored;
			if (room < 0) {
				room = 0;
			}
			int workshopResource = Data_Buildings[i].subtype.workshopResource;
			Data_CityInfo.resourceWorkshopRawMaterialSpace[workshopResource] += room;
			Data_CityInfo.resourceWorkshopRawMaterialStored[workshopResource] += Data_Buildings[i].loadsStored;
		}
	}
}

void Resource_addToCityWarehouses(int resource, int amount)
{
	int buildingId = Data_CityInfo.resourceNextTargetWarehouse;
	for (int i = 1; i < MAX_BUILDINGS; i++) {
		if (++buildingId >= MAX_BUILDINGS) {
			buildingId = 1;
		}
		if (Data_Buildings[buildingId].inUse == 1 &&
			Data_Buildings[buildingId].type == Building_Warehouse) {
			Data_CityInfo.resourceNextTargetWarehouse = buildingId;
			while (amount && Resource_addToWarehouse(buildingId, resource)) {
				amount--;
			}
		}
	}
}

int Resource_removeFromCityWarehouses(int resource, int amount)
{
	int amountLeft = amount;
	int buildingId = Data_CityInfo.resourceNextTargetWarehouse;
	// first go for non-getting warehouses
	for (int i = 1; i < MAX_BUILDINGS; i++) {
		if (++buildingId >= MAX_BUILDINGS) {
			buildingId = 1;
		}
		if (Data_Buildings[buildingId].inUse == 1 &&
			Data_Buildings[buildingId].type == Building_Warehouse) {
			int storageId = Data_Buildings[buildingId].storageId;
			if (Data_Building_Storages[storageId].resourceState[resource] != BuildingStorageState_Getting) {
				Data_CityInfo.resourceNextTargetWarehouse = buildingId;
				amountLeft = Resource_removeFromWarehouse(buildingId, resource, amountLeft);
			}
		}
	}
	// if that doesn't work, take it anyway
	for (int i = 1; i < MAX_BUILDINGS; i++) {
		if (++buildingId >= MAX_BUILDINGS) {
			buildingId = 1;
		}
		if (Data_Buildings[buildingId].inUse == 1 &&
			Data_Buildings[buildingId].type == Building_Warehouse) {
			Data_CityInfo.resourceNextTargetWarehouse = buildingId;
			amountLeft = Resource_removeFromWarehouse(buildingId, resource, amountLeft);
		}
	}
	return amount - amountLeft;
}

int Resource_addToWarehouse(int buildingId, int resource)
{
	// TODO
	return 0;
}

int Resource_removeFromWarehouse(int buildingId, int resource, int amount)
{
	// TODO
	return 0;
}


int Resource_getAmountStoredInWarehouse(int buildingId, int resource)
{
	int loads = 0;
	for (int i = 0; i < 8; i++) {
		buildingId = Data_Buildings[buildingId].nextPartBuildingId;
		if (buildingId <= 0) {
			return 0;
		}
		if (resource && Data_Buildings[buildingId].subtype.warehouseResourceId == resource) {
			loads += Data_Buildings[buildingId].loadsStored;
		}
	}
	return loads;
}

int Resource_getWarehouseSpaceInfo(int buildingId)
{
	int totalLoads = 0;
	int emptySpaces = 0;
	for (int i = 0; i < 8; i++) {
		buildingId = Data_Buildings[buildingId].nextPartBuildingId;
		if (buildingId <= 0) {
			return 0;
		}
		if (Data_Buildings[buildingId].subtype.warehouseResourceId) {
			totalLoads += Data_Buildings[buildingId].loadsStored;
		} else {
			emptySpaces++;
		}
	}
	if (emptySpaces > 0) {
		return 0; // room available
	} else if (totalLoads < 32) {
		return 2; // some room available for existing goods
	} else {
		return 1; // full
	}
}

int Resource_getGraphicIdOffset(int resource, int type)
{
	if (resource == Resource_Meat && Data_Scenario.allowedBuildings.wharf) {
		switch (type) {
			case 0: return 40;
			case 1: return 648;
			case 2: return 8;
			case 3: return 11;
			default: return 0;
		}
	} else {
		return 0;
	}
}