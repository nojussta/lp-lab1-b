#include <iostream>
#include <iomanip>
#include <fstream>
#include <array>
#include <algorithm>
#include "json.hpp"
#include "sha1.hpp"
#include <omp.h>

using namespace std;
using json = nlohmann::json;
const int DataSize = 16;

class PerformanceData
{
	double performanceScore;

public:
	PerformanceData()
	{
		this->performanceScore = 0;
	}
	PerformanceData(double value)
	{
		this->performanceScore = value;
	}
	double getPerformanceScore() const
	{
		return this->performanceScore;
	}
	void setPerformanceScore(double value)
	{
		this->performanceScore = value;
	}
};

struct Car {
	string make;
	double consumption;
	int power;
	string hashCode;
	PerformanceData performanceScore;
};

// Function to calculate the performance score
double calculatePerformanceScore(const Car& car) {
	return car.power / car.consumption;
}

// Function to check if a car meets the filter criteria
bool meetsFilterCriteria(Car& car, double filterThreshold) {
	return car.performanceScore.getPerformanceScore() > filterThreshold;
}

class ResultMonitor
{
	array<Car, DataSize> cars;
	int currentSize = 0;

public:
	// Get an array of cars
	array<Car, DataSize> getCars()
	{
		return cars;
	}

	// Add a car to the result array in a sorted manner
	void addSorted(Car newCar)
	{
#pragma omp critical
		{
			if (currentSize == 0)
			{
				cars[currentSize] = newCar;
				currentSize++;
			}
			else
			{
				cars[currentSize] = newCar;
				currentSize++;

				// Bubble sort by calculated performance score
				for (int i = 0; i < currentSize - 1; i++)
				{
					for (int j = 0; j < currentSize - i - 1; j++)
					{
						if (cars[j].performanceScore.getPerformanceScore() > cars[j + 1].performanceScore.getPerformanceScore())
						{
							// Swap cars[j] and cars[j + 1]
							Car temp = cars[j];
							cars[j] = cars[j + 1];
							cars[j + 1] = temp;
						}
					}
				}
			}
		}
	}

	//Get array size
	int getCount()
	{
		return currentSize;
	}
};

class OutputPrinter
{
public:
	// Print initial data to the console
	static void printInitialDataConsole(const array<Car, DataSize>& cars)
	{
		int dashCount = 70;

		cout << "Pradiniai duomenys" << endl;
		printDashedLineConsole(dashCount);

		cout << left << setw(23) << "| Make" << left << setw(23) << "| Consumption " << left << setw(23) << "| Performance " << "|" << endl;
		printDashedLineConsole(dashCount);

		for (const auto& car : cars)
		{
			cout << "| " << left << setw(20) << car.make
				<< " | " << left << setw(20) << car.consumption << " | "
				<< left << setw(20) << car.power << " | " << endl;
		}

		printDashedLineConsole(dashCount);
	}


	// Print initial data to the output file
	static void printInitialData(const array<Car, DataSize>& cars, string outputFile)
	{
		ofstream output(outputFile);
		int dashCount = 70;
		output << "Pradiniai duomenys" << endl;
		printDashedLine(output, dashCount);
		output << left << setw(23) << "| Make" << left << setw(23) << "| Consumption " << left << setw(23) << "| Performance " << "|" << endl;
		printDashedLine(output, dashCount);

		for (const auto& car : cars)
		{
			output << "| " << left << setw(20) << car.make
				<< " | " << left << setw(20) << car.consumption << " | "
				<< left << setw(20) << car.power << " | " << endl;
		}

		printDashedLine(output, dashCount);
		output << endl;
		output.close();
	}

	// Print results to the output file
	static void printResults(const array<Car, DataSize>& sortedCars, int count, string outputFile)
	{
		ofstream output(outputFile, ios_base::app);
		int dashCount = 136;
		output << "Rezultatai" << endl;
		printDashedLine(output, dashCount);
		output << left << setw(23) << "| Make" << left << setw(23) << "| Consumption " << left << setw(22) << "| Performance " << left << setw(23) << " | Performance score" << left << setw(44) << " | HashCode" << "|" << endl;
		printDashedLine(output, dashCount);

		for (int i = 0; i < count; i++)
		{
			output << "| " << left << setw(20) << sortedCars[i].make
				<< " | " << left << setw(20) << sortedCars[i].consumption << " | "
				<< left << setw(20) << sortedCars[i].power << " | "
				<< left << setw(20) << sortedCars[i].performanceScore.getPerformanceScore() << " | "
				<< left << setw(40) << sortedCars[i].hashCode << " |" << endl;
		}

		printDashedLine(output, dashCount);
		output.close();
	}

private:
	// Helper function to print dashes
	static void printDashedLineConsole(int count)
	{
		for (int i = 0; i < count; i++)
		{
			cout << "-";
		}
		cout << endl;
	}

	// Print a dashed line to the output file
	static void printDashedLine(ofstream& output, int size)
	{
		for (int i = 0; i < size; i++)
		{
			output << "-";
		}
		output << endl;
	}
};

ResultMonitor resultMonitor;
array<Car, DataSize> cars;

// Process data for a car and update the result monitor
double processCarData(Car& car, int threadId)
{
	for (int j = 0; j < 666666; j++)
	{
		/*999999
		666666*/
	}
	SHA1 sha1;
	sha1.update(car.make);
	sha1.update(std::to_string(car.consumption));
	sha1.update(std::to_string(car.power));
	car.hashCode = sha1.final();

	car.performanceScore = calculatePerformanceScore(car);

	// Check if the car meets the filter criteria and add to the result monitor
	if (meetsFilterCriteria(car, 19))
	{
		resultMonitor.addSorted(car);
		return car.power + car.consumption;
	}
	return 0;
}

int main()
{
	ifstream inputFile("duomenys.json");
	string jsonData((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
	json jsonCars = json::parse(jsonData);

	int i = 0;
	for (const auto& carData : jsonCars["cars"])
	{
		Car car;
		car.make = carData["make"];
		car.consumption = carData["consumption"];
		car.power = carData["power"];
		cars[i++] = car;
	}

	OutputPrinter::printInitialDataConsole(cars);
	OutputPrinter::printInitialData(cars, "rezultatai.txt");

	int size = cars.size();

	auto numbers = new double[DataSize];
	int numbersIndex = 0;
	double sum = 0;

	// Parallel processing using OpenMP
#pragma omp parallel for num_threads(6) reduction(+:sum)
	for (int i = 0; i < 6; i++)
	{
		int thread_id = omp_get_thread_num();
		int chunk_size = size / 6;

		int start = thread_id * chunk_size;
		int end = (thread_id == 5) ? size : start + chunk_size;

		for (int j = start; j < end; j++)
		{
			Car car = cars[j];
			numbers[numbersIndex++] = processCarData(car, thread_id);
			sum = accumulate(numbers + start, numbers + end, 0,
				[](double acc, double curr) { return acc + curr; });
		}
	}
	cout << sum << endl;

	array<Car, DataSize> sortedCars;
	sortedCars = resultMonitor.getCars();

	OutputPrinter::printResults(sortedCars, resultMonitor.getCount(), "rezultatai.txt");

	return 0;
}
