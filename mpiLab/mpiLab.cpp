#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <windows.h>


using namespace std;


//RUN cmd: mpiexec -n 11 mpiLab.exe

// Funkcja do losowego głosowania
int Glosuj(const vector<int>& votes) {
    // Losowe wybranie jednego z kandydatów
    return votes[rand() % votes.size()];
    //cout << "elo";
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int temp = 0;

    unordered_map<int, int> voteCounts;

    // Inicjalizacja generatora liczb losowych
    srand(static_cast<unsigned int>(time(nullptr)) + rank);

    // Tablica na liczby głosów
    vector<int> votes(size, 0);

    // Proces główny (SIEĆ)
    if (rank == 0) {
        
        int votedCandidate;
        MPI_Status status;

        int expectedSize;

        MPI_Recv(&expectedSize, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);

        while (expectedSize > 1) {
            MPI_Recv(&votedCandidate, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);

            for (int j = (((size - 1) / 2) + 1); j < size; j++) {
                MPI_Send(&votedCandidate, 1, MPI_INT, j, 2, MPI_COMM_WORLD);
            }
            MPI_Recv(&expectedSize, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
        }
    }


    // Procesy glosowanie
    if (0 < rank && rank <= (size - 1) / 2) {
        vector<int> votes;
        MPI_Status status;

        int idpg = rank + ((size - 1) / 2);
        int expectedSize = size;

        MPI_Recv(&expectedSize, 1, MPI_INT, idpg, 90, MPI_COMM_WORLD, &status);

        votes.resize(expectedSize);

        MPI_Recv(votes.data(), expectedSize, MPI_INT, idpg, 100, MPI_COMM_WORLD, &status);

        while (expectedSize != 1) {
            int votedCandidate = Glosuj(votes);

            MPI_Send(&votedCandidate, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);   
            MPI_Recv(&expectedSize, 1, MPI_INT, idpg, 90, MPI_COMM_WORLD, &status);

            votes.resize(expectedSize);

            MPI_Recv(votes.data(), expectedSize, MPI_INT, idpg, 100, MPI_COMM_WORLD, &status);
        }
    }


    // Licz
    if ((size - 1) / 2 < rank && rank <= size) {

        int election_round = 0;
        int N = 0;;
        int temp = 0;
        int id = 0;
        int expectedSize = 0;
        int votedCandidate;
        MPI_Status status;

        //Initialization of the first voting participants
        if (temp == 0) {
            vector<int> votes((size - 1) / 2, 0);

            for (int i = 0; i < (size - 1) / 2; ++i) {
                votes[i] = i + 1;
            }

            id = rank - ((size - 1) / 2);
            expectedSize = votes.size();
            MPI_Send(&expectedSize, 1, MPI_INT, 0, 99, MPI_COMM_WORLD);
            MPI_Send(&expectedSize, 1, MPI_INT, id, 90, MPI_COMM_WORLD);
            MPI_Send(votes.data(), expectedSize, MPI_INT, id, 100, MPI_COMM_WORLD);
            
            temp++;
        }

        while (expectedSize != 1) {
        
            MPI_Recv(&votedCandidate, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

            voteCounts[votedCandidate]++;

            //Wait until everyone votes
            N = N + 1;

            if (N == (size - 1) / 2) {

                N = 0;
                election_round++;
                votes.clear();

                for (auto it = voteCounts.begin(); it != voteCounts.end(); ) {
                    if (it->second == 0) {
                        it = voteCounts.erase(it);
                    }
                    else {
                        ++it;
                    }
                }

                auto minElement = min_element(
                    voteCounts.begin(), voteCounts.end(),
                    [](const auto& lhs, const auto& rhs) {
                        return lhs.second < rhs.second;
                    }
                );

                for (const auto& pair : voteCounts) {
                    int key = pair.first;
                    int value = pair.second;

                    if (value > minElement->second) {
                        votes.push_back(key);
                    }
                }


                //Showing voting results of the round
                for (int rank_number = ((size - 1) / 2) + 1; rank_number < size; rank_number++) {
                    if (rank == rank_number) {

                        Sleep(50 * rank);
                        cout << "Voting results for the round: " << election_round << " [PROCES LICZ: " << rank << "]" << endl;

                        for (const auto& pair : voteCounts) {
                            cout << "Key: " << pair.first << ", Value: " << pair.second << endl;
                        }
                    }
                }

                //wyswietlenie nowego wektora
                for (int ii = 0; ii < votes.size(); ++ii) {
                    Sleep(50 * rank);
                    if (rank == ((size - 1) / 2) + 1) {
                        cout << "Process: " << votes[ii] << " won in the " << election_round << "nd round." << endl;
                    }
                }

                sort(votes.begin(), votes.end());

                voteCounts.clear();

                id = rank - ((size - 1) / 2);
                expectedSize = votes.size();

                MPI_Send(&expectedSize, 1, MPI_INT, id, 90, MPI_COMM_WORLD);
                MPI_Send(votes.data(), expectedSize, MPI_INT, id, 100, MPI_COMM_WORLD);
                MPI_Send(&expectedSize, 1, MPI_INT, 0, 99, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
    return 0;
}