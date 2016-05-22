#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

#include <pb.h>
#include <pb_decode.h>

#include "stuffer.h"
#include "proto/telemetry.pb.h"

#define N 100000
#define BLOCK_SIZE 512
#define MAX_PACKET_SIZE 1024

using namespace std;

int runTest() {
	uint8_t data[N];
	uint8_t out_data[2 * N];
	uint8_t unstuffed_data[N];

	srand(time(NULL));
	for (int i = 0; i < N; ++i) {
		data[i] = rand() % 256;
	}

	int final_length = stuff_data(data, N, out_data);
	int unstuffed_length = unstuff_data(out_data, final_length, unstuffed_data);

	if (unstuffed_length != N) {
		cout << "Incorrect length!" << endl;
		return -1;
	}

	for (int i = 0; i < N; ++i) {
		if (data[i] != unstuffed_data[i]) {
			cout << "Error stuffing: " << i << endl;
			for (int i = 0; i < N; ++i) {
				cout << (unsigned int)data[i] << " ";
			}
			cout << endl;
			for (int i = 0; i < N; ++i) {
				cout << (unsigned int)unstuffed_data[i] << " ";
			}
			cout << endl;
			return -1;
		}
	}
}

bool isLogFinished(uint8_t *buf) {
	for (int i = 0; i < BLOCK_SIZE; ++i) {
		if (buf[i] != 0) {
			return false;
		}
	}
	return true;
}

vector <TelemetryPacket> readPackets(ifstream &in) {
	static uint8_t buf[BLOCK_SIZE];
	static uint8_t cur_packet[MAX_PACKET_SIZE];
	static uint8_t cur_packet2[MAX_PACKET_SIZE];
	int cur_packet_size = 0;
	int packet_id = 1;
	vector <TelemetryPacket> packets;
	TelemetryPacket packet;

	in.read(reinterpret_cast<char*>(buf), BLOCK_SIZE);
	while (!isLogFinished(buf)) {
		int i = 0;
		while (i < BLOCK_SIZE) {
			while (buf[i] && i < BLOCK_SIZE) {
				cur_packet[cur_packet_size++] = buf[i];
				++i;
			}
			// We've received a complete packet
			if (i < BLOCK_SIZE) {
				uint8_t checksum = 0;
				int len = unstuff_data(cur_packet, cur_packet_size + 1, cur_packet2);
				for (int j = 0; j + 1 < len; ++j) {
					checksum ^= cur_packet2[j];
				}
				if (checksum != cur_packet2[len - 1]) {
					cout << "Packet #" << packet_id << " crc fail!" << endl;
				} else {
					pb_istream_t stream;
					stream = pb_istream_from_buffer(cur_packet2, len - 1);
					if (pb_decode(&stream, TelemetryPacket_fields, &packet)) {
						packets.push_back(packet);
					} else {
						cout << "Failed to decode packet !" << packet_id << endl;
					}
				}
				++packet_id;
				cur_packet_size = 0;
				++i;
			}
		}
		in.read(reinterpret_cast<char*>(buf), BLOCK_SIZE);
	}

	return packets;
}

string packetToCSVRecord(const TelemetryPacket &packet) {
	stringstream ss;

	ss << packet.packet_id << ",";
	ss << packet.status << ",";
	ss << packet.timestamp << ",";

	if (packet.has_cpu_temperature) ss << packet.cpu_temperature;
	ss << ",";

	if (packet.has_altitude) ss << packet.altitude;
	ss << ",";

	if (packet.has_latitude) ss << packet.latitude;
	ss << ",";

	if (packet.has_longitude) ss << packet.longitude;
	ss << ",";

	if (packet.has_pressure) ss << packet.pressure;
	ss << ",";

	if (packet.has_bmp180_temperature) ss << packet.bmp180_temperature;
	ss << ",";

	if (packet.has_voltage) ss << packet.voltage;
	ss << ",";

	if (packet.has_radiation) ss << packet.radiation;
	ss << ",";

	if (packet.has_ds18b20_temperature1) ss << static_cast<int16_t>(packet.ds18b20_temperature1) / 16.0;
	ss << ",";

	if (packet.has_ds18b20_temperature2) ss << static_cast<int16_t>(packet.ds18b20_temperature2) / 16.0;
	ss << ",";

	if (packet.has_ds18b20_temperature3) ss << static_cast<int16_t>(packet.ds18b20_temperature3) / 16.0;
	ss << ",";

	if (packet.has_ds18b20_temperature4) ss << static_cast<int16_t>(packet.ds18b20_temperature4) / 16.0;
	ss << ",";

	if (packet.has_acceleration_x) ss << packet.acceleration_x;
	ss << ",";

	if (packet.has_acceleration_y) ss << packet.acceleration_y;
	ss << ",";

	if (packet.has_acceleration_z) ss << packet.acceleration_z;
	ss << ",";

	if (packet.has_gyroscope_x) ss << packet.gyroscope_x;
	ss << ",";

	if (packet.has_gyroscope_y) ss << packet.gyroscope_y;
	ss << ",";

	if (packet.has_gyroscope_z) ss << packet.gyroscope_z;
	ss << ",";

	if (packet.has_sun_sensor1) ss << packet.sun_sensor1;
	ss << ",";

	if (packet.has_sun_sensor2) ss << packet.sun_sensor2;
	ss << ",";

	if (packet.has_sun_sensor3) ss << packet.sun_sensor3;
	ss << ",";

	if (packet.has_sun_sensor4) ss << packet.sun_sensor4;
	ss << ",";

	if (packet.has_magnetometer_x) ss << packet.magnetometer_x;
	ss << ",";

	if (packet.has_magnetometer_y) ss << packet.magnetometer_y;
	ss << ",";

	if (packet.has_magnetometer_z) ss << packet.magnetometer_z;
	ss << ",";

	if (packet.has_quaternion0) ss << packet.quaternion0;
	ss << ",";

	if (packet.has_quaternion1) ss << packet.quaternion1;
	ss << ",";

	if (packet.has_quaternion2) ss << packet.quaternion2;
	ss << ",";

	if (packet.has_quaternion3) ss << packet.quaternion3;
	ss << ",";

	if (packet.has_ozone) ss << packet.ozone;
	ss << ",";

	if (packet.has_uv_light) ss << packet.uv_light;

	return ss.str();
}

string getCSVHead() {
	string res;

	res += "packet_id,";
	res += "status,";
	res += "timestamp,";
	res += "cpu_temperature,";
	res += "altitude,";
	res += "latitude,";
	res += "longitude,";
	res += "pressure,";
	res += "bmp180_temperature,";
	res += "voltage,";
	res += "radiation,";
	res += "ds18b20_temperature1,";
	res += "ds18b20_temperature2,";
	res += "ds18b20_temperature3,";
	res += "ds18b20_temperature4,";
	res += "acceleration_x,";
	res += "acceleration_y,";
	res += "acceleration_z,";
	res += "gyroscope_x,";
	res += "gyroscope_y,";
	res += "gyroscope_z,";
	res += "sun_sensor1,";
	res += "sun_sensor2,";
	res += "sun_sensor3,";
	res += "sun_sensor4,";
	res += "magnetometer_x,";
	res += "magnetometer_y,";
	res += "magnetometer_z,";
	res += "quaternion0,";
	res += "quaternion1,";
	res += "quaternion2,";
	res += "quaternion3,";
	res += "ozone,";
	res += "uv_light";

	return res;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Expected log file name" << endl;
		return -1;
	}

	ifstream in(argv[1], ios::in | ios::binary);
	if (!in) {
		cerr << "Failed to open the file" << endl;
		return -1;
	}
	auto packets = readPackets(in);

	ofstream out("data.csv");
	out << getCSVHead() << endl;
	for (const auto &p: packets) {
		out << packetToCSVRecord(p) << endl;
	}
	return 0;
}
