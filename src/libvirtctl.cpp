#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

typedef int DOMAIN_INFO_STATUS;
typedef int DOMAIN_INFO_LOOKUPID;

static virConnectPtr connection = NULL;
static virDomainPtr domainInfoPtr = NULL;
static DOMAIN_INFO_STATUS domainStart(const char * guestname);
static DOMAIN_INFO_STATUS domainShutdown(const char * guestname);
static DOMAIN_INFO_STATUS domainForcedShutdown(const char * guestname);
//static DOMAIN_INFO_STATUS GetDomainInformation(DOMAIN_INFO_LOOKUPID id);
static DOMAIN_INFO_STATUS domainReboot(const char * guestname);
static DOMAIN_INFO_STATUS domainReset(const char * guestname);
static DOMAIN_INFO_STATUS domainResume(const char * guestname);
static DOMAIN_INFO_STATUS GetDomainInformation(const char * guestname);

/*
virConnectPtr getConnection() {
	connection = virConnectOpenReadOnly(NULL);
	if(connection == NULL)
	{
		printf("error!\n");
		exit(1);
	}
	return connection;
}

DOMAIN_INFO_STATUS GetDomainInformation(DOMAIN_INFO_LOOKUPID id) {
	virDomainInfo domainInfo;
	connection = getConnection();
	domainInfoPtr = virDomainLookupByID(connection, id);
	if(domainInfoPtr == NULL)
	{
		printf("Can not find Domain!\n");
		virConnectClose(connection);
		exit(1);
	}
	if( virDomainGetInfo(domainInfoPtr, &domainInfo) < 0 )
	{
		printf("Can not get Information!\n");
		virDomainFree(domainInfoPtr);
		exit(1);
	}
	
	printf("The Domain Status is : %d.\n", domainInfo.state);
	printf("The Domain Allowed Max Memory is : %ld.\n", domainInfo.maxMem);
	printf("The Domain Memory is : %ld. \n", domainInfo.memory);
	printf("The Domain Virtual CPU is : %d.\n", domainInfo.nrVirtCpu);

	if(domainInfoPtr != NULL)
	{
		virDomainFree(domainInfoPtr);
	}
	if(connection != NULL)
	{
		virConnectClose(connection);
	}
}*/

int main(int argc, const char * argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: ./libvirtctl guestName(domainName) start(START)/shutdown(SHUTDOWN)/forcedShutdown(FORCEDSHUTDOWN)/reboot(REBOOT)/reset(RESET)/resume(RESUME)/status(STATUS).\n");
		return -1;
	}
	connection = virConnectOpen("qemu:///system");
	if (connection == NULL) {
		fprintf(stderr, "Failed to open connectionn to qemu:///system.\n");
		return -1;
	}
	domainInfoPtr = virDomainLookupByName(connection, argv[1]);
	if (domainInfoPtr == NULL)
	{
		fprintf(stderr, "virDomainLookupByName failed.\n");
		virConnectClose(connection);
		return -1;
	}

	if (strcmp(argv[2], "start") == 0 || strcmp(argv[2], "START") == 0 ) {
		if (domainStart(argv[1]) != 0 ) {
			fprintf(stderr, "Start Failed.\n");
			virDomainFree(domainInfoPtr);
			return -1;
		}
	}

	if (strcmp(argv[2], "shutdown") == 0 || strcmp(argv[2], "SHUTDOWN") == 0 ) {
		if (domainShutdown(argv[1]) != 0 ) {
			fprintf(stderr, "Shutdown Failed.\n");
			virConnectClose(connection);
			virDomainFree(domainInfoPtr);
			return -1;
		}
	}

	if(strcmp(argv[2], "forcedShutdown") == 0 || strcmp(argv[2], "FORCEDSHUTDOWN") == 0 ) {
		if(domainForcedShutdown(argv[1]) != 0) {
			fprintf(stderr, "Forced Shutdown Failed.\n");
			virConnectClose(connection);
			virDomainFree(domainInfoPtr);
			return -1;
		}
	}

	if(strcmp(argv[2], "reboot") == 0 || strcmp(argv[2], "REBOOT") == 0) {
		if(domainReboot(argv[1]) != 0) {
			fprintf(stderr, "Reboot Failed.\n");
			virConnectClose(connection);
			virDomainFree(domainInfoPtr);
			return -1;
		}
	}

	if(strcmp(argv[2], "reset") == 0 || strcmp(argv[2], "RESET") == 0) {
		if(domainReset(argv[1]) != 0) {
			fprintf(stderr, "Reset Failed.\n");
			virConnectClose(connection);
			virDomainFree(domainInfoPtr);
			return -1;
		}
	}

	if(strcmp(argv[2], "resume") == 0 || strcmp(argv[2], "RESUME") == 0) {
		if(domainResume(argv[1]) != 0) {
			fprintf(stderr, "Resume Failed.\n");
			virConnectClose(connection);
			virDomainFree(domainInfoPtr);
			return -1;
		}
	}

	if (strcmp(argv[2], "status") == 0 || strcmp(argv[2], "STATUS") == 0 ) {
		if (GetDomainInformation(argv[1]) != 0 ) {
			fprintf(stderr, "Get Status Failed.\n");
			virDomainFree(domainInfoPtr);
			virConnectClose(connection);
			return -1;
		}
	}

/*	if(strcmp(argv[2], "start") != 0 || strcmp(argv[2], "shutdown") !=0 || strcmp(argv[2], "status") !=0 || strcmp(argv[2], "START") != 0 || strcmp(argv[2], "SHUTDOWN") != 0 || strcmp(argv[2], "STATUS") != 0)
	{
		fprintf(stderr, "Usage: ./libvirtctl guestName(domainName) start(START)/shutdown(SHUTDOWN)/status(STATUS).\n");
		return -1;
	}*/

	if (domainInfoPtr != NULL)
	{
		virDomainFree(domainInfoPtr);
	}
	if (connection != NULL)
	{
		virConnectClose(connection);
	}
	
	return EXIT_SUCCESS;
}

DOMAIN_INFO_STATUS domainStart(const char * guestname)
{
	int flag = -1;
	flag = virDomainCreate(domainInfoPtr);
	if (flag != 0)
	{
		virErrorPtr error = virGetLastError();
		fprintf(stderr, "virDomainCreate failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	else
	{
		fprintf(stdout, "libvirtctl: %s domain starting was succeed.\n", guestname);
	}
	return 0;
}

DOMAIN_INFO_STATUS domainShutdown(const char * guestname)
{
	int flag = -1;
	flag = virDomainShutdown(domainInfoPtr);
	if (flag != 0)
	{
		virErrorPtr error = virGetLastError();
		fprintf(stderr, "virDomainShutdown failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	else
	{
		fprintf(stdout, "libvirtctl: %s domain shutdown was succeed.\n", guestname);

	}
	return 0;
}

DOMAIN_INFO_STATUS domainForcedShutdown(const char * guestname)
{
	int flag = -1;
	domainShutdown(guestname);
	flag = virDomainDestroy(domainInfoPtr);
	if(flag != 0)
	{
		virErrorPtr error = virGetLastError();
		fprintf(stderr, "virDomainForcedShutdown failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	else
	{
		fprintf(stdout, "libvirtctl: %s domain forced shutdown was succeed.\n", guestname);
	}
	return 0;
}

DOMAIN_INFO_STATUS domainReboot(const char * guestname)
{
	int flag = -1;
	flag = virDomainReboot(domainInfoPtr, VIR_DOMAIN_REBOOT_ACPI_POWER_BTN);
	if(flag != 0) 
	{
		virErrorPtr error = virGetLastError();
		fprintf(stderr, "virDomainReboot failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	else
	{
		fprintf(stdout, "libvirtctl: %s domain reboot was succeed.\n", guestname);
	}
	return 0;
}

DOMAIN_INFO_STATUS domainReset(const char * guestname)
{
	int flag = -1;
	flag = virDomainReset(domainInfoPtr, 0);
	if(flag != 0)
	{
		virErrorPtr error = virGetLastError();
		fprintf(stderr, "virDomainReset failed: %s.\n", guestname);
		virFreeError(error);
		return -1;
	}
	else
	{
		fprintf(stdout, "libvirtctl: %s domain reset was succeed.\n", guestname);
	}
	return 0;
}

DOMAIN_INFO_STATUS domainResume(const char * guestname)
{
	int flag = -1;
	flag = virDomainResume(domainInfoPtr);
	if(flag != 0)
	{
		virErrorPtr error = virGetLastError();
		fprintf(stderr, "virDomainResume failed: %s.\n", guestname);
		virFreeError(error);
		return -1;
	}
	else
	{
		fprintf(stdout, "libvirtctl: %s domain resume was succeed.\n", guestname);
	}
	return 0;
}

DOMAIN_INFO_STATUS GetDomainInformation(const char * guestname)
{
	char * status = NULL;
	virErrorPtr error = NULL;
	int vcpus = 0;
	unsigned long long node_free_memory = 0;
	int id = 0;
	const char * name = NULL;
	virNodeInfo nodeInfo;
	virDomainInfo domainInfo;

	fprintf(stdout, "****************************************************\n");
	status = virConnectGetCapabilities(connection);
	if (status == NULL)
	{
		error = virGetLastError();
		fprintf(stderr, "virConnectGetCapabilities failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	free(status);
	status = NULL;

	status = virConnectGetHostname(connection);
	if (status == NULL)
	{
		error = virGetLastError();
		fprintf(stderr, "virConnectGetHostname failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "Connection Hostname:\t%s\n", status);
	free(status);
	status = NULL;

	vcpus = virConnectGetMaxVcpus(connection, NULL);
	if (vcpus < 0)
	{
		error = virGetLastError();
		fprintf(stderr, "virConnectGetMaxVcpus faild: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "Maximum number of cpus supported on connection:\t%d\n", vcpus);
	
	node_free_memory = virNodeGetFreeMemory(connection);
	if (node_free_memory == 0) {
		error = virGetLastError();
		fprintf(stderr, "virNodeGetFreeMemory failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "Node Free Memory:\t%llu\n", node_free_memory);
	
	if (virNodeGetInfo(connection, &nodeInfo) < 0) {
		error = virGetLastError();
		fprintf(stderr, "virNodeGetInfo failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}

	fprintf(stdout, "------------------------------------------\n");
	fprintf(stdout, "Node Information From Connection : \n");
	fprintf(stdout, "Model:\t%s\n", nodeInfo.model);
	fprintf(stdout, "Memory Size:\t%luKiB\n", nodeInfo.memory);
	fprintf(stdout, "Number of CPUs:\t%u\n", nodeInfo.cpus);
	fprintf(stdout, "MHz of CPUs:\t%u\n", nodeInfo.mhz);
	fprintf(stdout, "Number of NUMA Nodes:\t%u\n", nodeInfo.nodes);
	fprintf(stdout, "Number of CPU Sockets:\t%u\n", nodeInfo.sockets);
	fprintf(stdout, "Number of CPU Cores Per Socket:\t%u\n", nodeInfo.cores);
	fprintf(stdout, "Number of CPU Threads Per Core:\t%u\n", nodeInfo.threads);
	fprintf(stdout, "****************************************************\n");
	fprintf(stdout, "ID\t名称\t\t状态\n");
	fprintf(stdout, "------------------------------------------\n");
	id = virDomainGetID(domainInfoPtr);
	name = virDomainGetName(domainInfoPtr);
	if (virDomainGetInfo(domainInfoPtr, &domainInfo) < 0) {
		error = virGetLastError();
		fprintf(stderr, "virDomainGetInfo failed: %s.\n", error->message);
		virFreeError(error);
		return -1;
	}
	fprintf(stdout, "%d\t%s\t\t%d\n", id, name, domainInfo.state);
	fprintf(stdout, "****************************************************\n");
	return 0;
}
