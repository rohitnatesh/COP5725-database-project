#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>

int bdOneAlgorithm(long vertices, long edgesCount, std::vector<long> &startIndices, std::vector<long> &edges)
{
	std::vector<char> isUnvisited(vertices, 1);
	std::vector<int> degreesLL(vertices, -1);
	std::vector<int> degreeLLChain(vertices, -1);
	std::vector<int> degrees(vertices, 0);
	std::vector<long> degreeOnes, peeledVertices;
	int maxDegree = 0, independentSetSize = 0;

	for (long i = 0; i < vertices; i++)
	{
		long vertexDegree = startIndices[i + 1] - startIndices[i];
		degrees[i] = vertexDegree;
		degreeLLChain[i] = degreesLL[vertexDegree];
		degreesLL[vertexDegree] = i;

		if (vertexDegree == 0)
		{
			independentSetSize += 1;
		}
		else if (vertexDegree == 1)
		{
			degreeOnes.push_back(i);
		}

		if (vertexDegree > maxDegree)
		{

			maxDegree = degrees[i];
		}
	}

	while (!degreeOnes.empty() || maxDegree > 1)
	{
		while (!degreeOnes.empty())
		{
			long u = degreeOnes[degreeOnes.size() - 1];
			degreeOnes.erase(degreeOnes.begin() + degreeOnes.size() - 1);

			for (int j = startIndices[u]; j < startIndices[u + 1] && isUnvisited[u] && degrees[u] == 1; j++)
			{
				long v = edges[j];

				if (isUnvisited[v])
				{
					isUnvisited[v] = 0;

					for (int k = startIndices[v]; k < startIndices[v + 1]; k++)
					{
						int w = edges[k];

						if (isUnvisited[w])
						{
							degrees[w] -= 1;

							if (degrees[w] == 0)
							{
								independentSetSize += 1;
							}
							else if (degrees[w] == 1)
							{
								degreeOnes.push_back(w);
							}
						}
					}
				}
			}
		}

		while (degreeOnes.empty())
		{
			while (maxDegree > 1 && degreesLL[maxDegree] == -1)
			{
				maxDegree -= 1;
			}

			if (maxDegree < 2)
			{
				break;
			}

			int v = degreesLL[maxDegree];

			while (v != -1)
			{
				int nextV = degreeLLChain[v], hasPeeled = 0;

				if (isUnvisited[v] && degrees[v] > 0)
				{
					if (degrees[v] < maxDegree)
					{
						degreeLLChain[v] = degreesLL[degrees[v]];
						degreesLL[degrees[v]] = v;
					}
					else
					{
						peeledVertices.push_back(v);
						isUnvisited[v] = 0;
						hasPeeled = 1;

						for (int k = startIndices[v]; k < startIndices[v + 1]; k++)
						{
							int w = edges[k];

							if (isUnvisited[w])
							{
								degrees[w] -= 1;
								if (degrees[w] == 0)
								{
									independentSetSize += 1;
								}
								else if (degrees[w] == 1)
								{
									degreeOnes.push_back(w);
								}
							}
						}

						degreesLL[maxDegree] = nextV;
					}
				}

				if (hasPeeled)
				{
					break;
				}

				v = nextV;
			}

			if (v == -1)
			{
				degreesLL[maxDegree] = -1;
			}
		}
	}

	for (long u : peeledVertices)
	{
		int isIndependentVertex = 1;

		for (long j = startIndices[u]; j < startIndices[u + 1] && isIndependentVertex; j++)
		{
			if (isUnvisited[edges[j]])
			{
				isIndependentVertex = 0;
			}
		}

		if (isIndependentVertex)
		{
			isUnvisited[u] = 1;
			independentSetSize += 1;
		}
	}

	return independentSetSize;
}

int bdTwoAlgorithm(long vertices, long edgesCount, const std::vector<long> &startIndices, const std::vector<long> &edges)
{
	std::vector<std::vector<int>> adjacencyList(edgesCount, std::vector<int>(3));
	std::vector<int> head(vertices, -1);
	std::vector<char> isInIndependentSet(vertices, 1);
	std::vector<int> degreeLL(vertices, -1);
	std::vector<int> degreeLLNextChain(vertices);
	std::vector<int> degreeLLPreviousChain(vertices, -1);
	std::vector<int> degree(vertices);
	std::vector<int> includes(vertices, -1);
	std::vector<int> excludes(vertices, -1);
	std::vector<long> degreeOnes, degreeTwos;
	std::vector<char> isIn(vertices, 0);
	std::vector<char> isContracted(vertices, 0);

	int count = 0;
	for (long i = 0; i < vertices; i++)
	{
		for (long j = startIndices[i]; j < startIndices[i + 1]; j++)
		{
			int v = edges[j];

			if (v <= i)
				continue;

			adjacencyList[count] = {v, count + 1, head[i]};
			head[i] = count++;

			adjacencyList[count] = {i, count - 1, head[v]};
			head[v] = count++;
		}
	}

	int maxDegree = 0, independentSetSize = 0;

	for (long i = 0; i < vertices; i++)
	{
		degree[i] = startIndices[i + 1] - startIndices[i];
		degreeLLNextChain[i] = degreeLL[degree[i]];

		if (degreeLL[degree[i]] != -1)
		{
			degreeLLPreviousChain[degreeLL[degree[i]]] = i;
		}

		degreeLL[degree[i]] = i;

		switch (degree[i])
		{
		case 0:
			independentSetSize += 1;
			break;
		case 1:
			degreeOnes.push_back(i);
			break;
		case 2:
			degreeTwos.push_back(i);
			break;
		}

		if (degree[i] > maxDegree)
		{
			maxDegree = degree[i];
		}
	}

	for (; !degreeOnes.empty() || !degreeTwos.empty() || maxDegree >= 3;)
	{
		for (; !degreeOnes.empty() || !degreeTwos.empty();)
		{
			for (; !degreeOnes.empty();)
			{
				int u = degreeOnes[degreeOnes.size() - 1];
				degreeOnes.resize(degreeOnes.size() - 1);

				if (!isInIndependentSet[u] || degree[u] != 1)
					continue;

				count = 0;
				int j = head[u];
				while (j != -1)
				{
					if (isInIndependentSet[adjacencyList[j][0]])
					{
						int v = adjacencyList[j][0];
						isInIndependentSet[v] = 0;

						int k = head[v];
						while (k != -1)
						{
							if (isInIndependentSet[adjacencyList[k][0]])
							{
								int w = adjacencyList[k][0];

								if (degreeLLPreviousChain[w] != -1)
									degreeLLNextChain[degreeLLPreviousChain[w]] = degreeLLNextChain[w];
								else
									degreeLL[degree[w]] = degreeLLNextChain[w];

								if (degreeLLNextChain[w] != -1)
									degreeLLPreviousChain[degreeLLNextChain[w]] = degreeLLPreviousChain[w];

								degree[w] -= 1;

								degreeLLNextChain[w] = degreeLL[degree[w]];
								degreeLLPreviousChain[w] = -1;
								if (degreeLL[degree[w]] != -1)
									degreeLLPreviousChain[degreeLL[degree[w]]] = w;
								degreeLL[degree[w]] = w;

								switch (degree[w])
								{
								case 0:
									independentSetSize += 1;
									break;
								case 1:
									degreeOnes.push_back(w);
									break;
								case 2:
									degreeTwos.push_back(w);
									break;
								}
							}
							k = adjacencyList[k][2];
						}
					}
					j = adjacencyList[j][2];
				}
			}

			while (degreeOnes.empty() && !degreeTwos.empty())
			{
				int u = degreeTwos[degreeTwos.size() - 1], v1 = -1, v2 = -1;
				;
				degreeTwos.resize(degreeTwos.size() - 1);

				if (!isInIndependentSet[u] || degree[u] != 2)
				{
					continue;
				}

				for (int j = head[u]; j != -1; j = adjacencyList[j][2])
				{
					if (isInIndependentSet[adjacencyList[j][0]])
					{
						if (v1 == -1)
						{
							v1 = adjacencyList[j][0];
						}
						else if (v2 == -1)
						{
							v2 = adjacencyList[j][0];
						}
					}
				}

				char found = 0;
				for (int j = head[v1]; j != -1; j = adjacencyList[j][2])
				{
					if (adjacencyList[j][0] == v2)
					{
						found = 1;
					}
				}

				if (found)
				{
					long v = v1;
					isInIndependentSet[v] = 0;

					for (int k = head[v]; k != -1; k = adjacencyList[k][2])
					{
						if (isInIndependentSet[adjacencyList[k][0]])
						{
							int w = adjacencyList[k][0];

							if (degreeLLPreviousChain[w] != -1)
							{
								degreeLLNextChain[degreeLLPreviousChain[w]] = degreeLLNextChain[w];
							}
							else
							{
								degreeLL[degree[w]] = degreeLLNextChain[w];
							}

							if (degreeLLNextChain[w] != -1)
							{
								degreeLLPreviousChain[degreeLLNextChain[w]] = degreeLLPreviousChain[w];
							}

							degree[w] -= 1;
							degreeLLNextChain[w] = degreeLL[degree[w]];
							degreeLLPreviousChain[w] = -1;

							if (degreeLL[degree[w]] != -1)
							{
								degreeLLPreviousChain[degreeLL[degree[w]]] = w;
							}

							degreeLL[degree[w]] = w;

							if (degree[w] == 0)
							{
								independentSetSize += 1;
							}
							else if (degree[w] == 1)
							{
								degreeOnes.push_back(w);
							}
							else if (degree[w] == 2)
							{
								degreeTwos.push_back(w);
							}
						}
					}

					long f = v2;
					isInIndependentSet[f] = 0;

					for (int k = head[f]; k != -1; k = adjacencyList[k][2])
					{
						if (isInIndependentSet[adjacencyList[k][0]])
						{
							int w = adjacencyList[k][0];

							if (degreeLLPreviousChain[w] != -1)
							{
								degreeLLNextChain[degreeLLPreviousChain[w]] = degreeLLNextChain[w];
							}
							else
							{
								degreeLL[degree[w]] = degreeLLNextChain[w];
							}
							if (degreeLLNextChain[w] != -1)
							{
								degreeLLPreviousChain[degreeLLNextChain[w]] = degreeLLPreviousChain[w];
							}
							--degree[w];

							degreeLLNextChain[w] = degreeLL[degree[w]];
							degreeLLPreviousChain[w] = -1;
							if (degreeLL[degree[w]] != -1)
							{
								degreeLLPreviousChain[degreeLL[degree[w]]] = w;
							}
							degreeLL[degree[w]] = w;

							if (degree[w] == 0)
							{
								independentSetSize += 1;
							}
							else if (degree[w] == 1)
							{
								degreeOnes.push_back(w);
							}
							else if (degree[w] == 2)
							{
								degreeTwos.push_back(w);
							}
						}
					}
				}
				else
				{
					int tmp = v1;
					isContracted[u] = isContracted[v2] = 1;
					isInIndependentSet[u] = isInIndependentSet[v2] = isContracted[u] - 1;

					for (; includes[tmp] != -1; tmp = includes[tmp]){}

					independentSetSize +=1;
					includes[tmp] = v2;

					switch (excludes[v1])
					{
					case -1:
						excludes[v1] = u;
						break;
					default:
						tmp = excludes[v1];
						while (includes[tmp] != -1)
						{
							tmp = includes[tmp];
						}
						includes[tmp] = u;
						break;
					}

					if (degreeLLPreviousChain[v1] == -1)
					{
						degreeLL[degree[v1]] = degreeLLNextChain[v1];
					}
					else
					{
						degreeLLNextChain[degreeLLPreviousChain[v1]] = degreeLLNextChain[v1];
					}

					degreeLLPreviousChain[degreeLLNextChain[v1]] = degreeLLNextChain[v1] != -1 ? degreeLLPreviousChain[v1] : degreeLLPreviousChain[degreeLLNextChain[v1]];

					int preDegree = degree[v1];
					--degree[v1];

					int last = -1;
					int j = head[v1];
					while (j != -1)
					{
						isIn[adjacencyList[j][0]] = 1;
						last = j;
						j = adjacencyList[j][2];
					}

					j = head[v2];
					while (j != -1)
					{
						if (isInIndependentSet[adjacencyList[j][0]])
						{
							if (!isIn[adjacencyList[j][0]])
							{
								adjacencyList[last][2] = j;
								last = j;
								++degree[v1];
								adjacencyList[adjacencyList[j][1]][0] = v1;
							}
							else
							{
								int w = adjacencyList[j][0];

								if (degreeLLPreviousChain[w] != -1)
								{
									degreeLLNextChain[degreeLLPreviousChain[w]] = degreeLLNextChain[w];
								}
								else
								{
									degreeLL[degree[w]] = degreeLLNextChain[w];
								}

								if (degreeLLNextChain[w] != -1)
								{
									degreeLLPreviousChain[degreeLLNextChain[w]] = degreeLLPreviousChain[w];
								}

								--degree[w];

								degreeLLNextChain[w] = degreeLL[degree[w]];
								degreeLLPreviousChain[w] = -1;
								if (degreeLL[degree[w]] != -1)
								{
									degreeLLPreviousChain[degreeLL[degree[w]]] = w;
								}

								degreeLL[degree[w]] = w;

								switch (degree[w])
								{
								case 1:
									degreeOnes.push_back(w);
									break;
								case 2:
									degreeTwos.push_back(w);
									break;
								}
							}
						}
						j = adjacencyList[j][2];
					}
					adjacencyList[last][2] = -1;

					j = head[v1];
					while (j != -1)
					{
						isIn[adjacencyList[j][0]] = 0;
						j = adjacencyList[j][2];
					}

					maxDegree = (degree[v1] > maxDegree) ? degree[v1] : maxDegree;

					degreeLLNextChain[v1] = degreeLL[degree[v1]];
					degreeLLPreviousChain[v1] = -1;

					degreeLLPreviousChain[degreeLL[degree[v1]]] = (degreeLL[degree[v1]] != -1) ? v1 : degreeLLPreviousChain[degreeLL[degree[v1]]];

					degreeLL[degree[v1]] = v1;

					degree[v1] == 1 && preDegree != 1 ? degreeOnes.push_back(v1) : (degree[v1] == 2 && preDegree != 2 ? degreeTwos.push_back(v1) : void());
				}
			}
		}

		while (degreeOnes.empty() && degreeTwos.empty() && maxDegree >= 3)
		{
			int u = degreeLL[maxDegree], nextDegree = degreeLLNextChain[u];

			if (u != -1)
			{
				degreeLL[maxDegree] = nextDegree;
				degreeLLPreviousChain[nextDegree] = nextDegree != -1 ? -1 : degreeLLPreviousChain[nextDegree];

				if (!isInIndependentSet[u])
				{
					continue;
				}

				long v = u;
				isInIndependentSet[v] = 0;

				for (int k = head[v]; k != -1; k = adjacencyList[k][2])
				{
					if (isInIndependentSet[adjacencyList[k][0]])
					{
						int w = adjacencyList[k][0];

						if (degreeLLPreviousChain[w] != -1)
						{
							degreeLLNextChain[degreeLLPreviousChain[w]] = degreeLLNextChain[w];
						}
						else
						{
							degreeLL[degree[w]] = degreeLLNextChain[w];
						}

						if (degreeLLNextChain[w] != -1)
						{
							degreeLLPreviousChain[degreeLLNextChain[w]] = degreeLLPreviousChain[w];
						}

						--degree[w];

						degreeLLNextChain[w] = degreeLL[degree[w]];
						degreeLLPreviousChain[w] = -1;
						if (degreeLL[degree[w]] != -1)
						{
							degreeLLPreviousChain[degreeLL[degree[w]]] = w;
						}
						degreeLL[degree[w]] = w;

						if (degree[w] == 0)
						{
							independentSetSize += 1;
						}
						else if (degree[w] == 1)
						{
							degreeOnes.push_back(w);
						}
						else if (degree[w] == 2)
						{
							degreeTwos.push_back(w);
						}
					}
				}
			}
			else
			{
				maxDegree -= 1;
			}
		}
	}

	for (long i = 0; i < vertices; i++)
	{
		if (!isContracted[i])
		{
			std::vector<long> backtrack;
			backtrack.push_back(i);
			while (!backtrack.empty())
			{
				long u = backtrack.back();
				backtrack.pop_back();

				if (isInIndependentSet[u])
				{
					if (includes[u] != -1)
					{
						isInIndependentSet[includes[u]] = 1;
						backtrack.push_back(includes[u]);
					}

					if (excludes[u] != -1)
					{
						isInIndependentSet[excludes[u]] = 0;
						backtrack.push_back(excludes[u]);
					}
				}
				else
				{
					if (includes[u] != -1)
					{
						isInIndependentSet[includes[u]] = 0;
						backtrack.push_back(includes[u]);
					}

					if (excludes[u] != -1)
					{
						isInIndependentSet[excludes[u]] = 1;
						backtrack.push_back(excludes[u]);
					}
				}
			}
		}
	}

	for (long i = 0; i < vertices; i++)
	{
		if (isInIndependentSet[i])
		{
			continue;
		}

		int found = 0;
		long j = startIndices[i];
		while (j < startIndices[i + 1])
		{
			if (isInIndependentSet[edges[j]])
			{
				found = 1;
				break;
			}
			j++;
		}

		if (!found)
		{
			++independentSetSize;
			isInIndependentSet[i] = 1;
		}
	}

	return independentSetSize;
}

int linearTimeAlgorithm(long numVertices, long numEdges, std::vector<long> &startIndices, std::vector<long> &edgeList)
{
	std::vector<char> isActive(numVertices, 1);
	std::vector<int> degreeLL(numVertices, -1), degreeLLChain(numVertices), vertexDegree(numVertices);
	std::vector<long> pending(numVertices);
	std::vector<long> degreeOneVertices, degreeTwoVertices;
	std::vector<std::pair<long, long>> stack;
	std::vector<std::pair<std::pair<long, long>, long>> modifiedEdges;

	int maxDegree = 0, independentSetSize = 0;
	long index = 0;

	while (index < numVertices)
	{
		int currentDegree = startIndices[index + 1] - startIndices[index];
		vertexDegree[index] = currentDegree;
		degreeLLChain[index] = degreeLL[currentDegree];
		degreeLL[currentDegree] = index;

		if (currentDegree == 0)
			independentSetSize = independentSetSize + 1;
		else if (currentDegree == 1)
			degreeOneVertices.push_back(index);
		else if (currentDegree == 2)
			degreeTwoVertices.push_back(index);

		if (currentDegree > maxDegree)
			maxDegree = currentDegree;

		index++;
	}

	std::vector<char> isFixed(numVertices, 0);

	index = 0;
	while (index < numVertices)
	{
		pending[index] = startIndices[index + 1];
		index++;
	}

	for (; !degreeOneVertices.empty() || !degreeTwoVertices.empty() || maxDegree >= 3;)
	{
		for (; !degreeOneVertices.empty() || !degreeTwoVertices.empty();)
		{
			for (; !degreeOneVertices.empty();)
			{
				long vertexU = degreeOneVertices.back();
				degreeOneVertices.pop_back();

				if (isActive[vertexU] && vertexDegree[vertexU] == 1)
				{
					int count = 0;
					for (long j = startIndices[vertexU]; j < pending[vertexU]; j++)
					{
						if (isActive[edgeList[j]])
						{
							++count;
							long vertexV = edgeList[j];
							isActive[vertexV] = 0;

							for (long k = startIndices[vertexV]; k < pending[vertexV]; k++)
							{
								if (isActive[edgeList[k]])
								{
									long vertexW = edgeList[k];
									--vertexDegree[vertexW];
									if (vertexDegree[vertexW] == 0)
										independentSetSize += 1;
									else if (vertexDegree[vertexW] == 1)
										degreeOneVertices.push_back(vertexW);
									else if (vertexDegree[vertexW] == 2)
										degreeTwoVertices.push_back(vertexW);
								}
							}
						}
					}
				}
			}

			while (!degreeTwoVertices.empty() && degreeOneVertices.empty())
			{
				long vertexU = degreeTwoVertices.back();
				degreeTwoVertices.pop_back();
				if (!isActive[vertexU] || vertexDegree[vertexU] != 2)
					continue;

				long &pendingEnd = pending[vertexU];
				long *triangle = nullptr;
				long edgeIndex = startIndices[vertexU];
				for (; edgeIndex < pendingEnd;)
				{
					for (; edgeIndex < pendingEnd && isActive[edgeList[edgeIndex]]; ++edgeIndex)
					{
					}
					for (; edgeIndex < pendingEnd && !isActive[edgeList[pendingEnd - 1]]; --pendingEnd)
					{
					}

					if (edgeIndex >= pendingEnd)
						break;
					std::swap(edgeList[edgeIndex], edgeList[pendingEnd - 1]);

					if (triangle != nullptr)
					{
						std::swap(triangle[edgeIndex], triangle[pendingEnd - 1]);
					}
				}

				long firstNeighbor = edgeList[startIndices[vertexU]], secondNeighbor = edgeList[startIndices[vertexU] + 1];
				long prevVertex = vertexU, chainLength = 1;
				while (firstNeighbor != vertexU && vertexDegree[firstNeighbor] == 2)
				{
					++chainLength;
					long currentVertex = firstNeighbor;
					long &currentEnd = pending[firstNeighbor];
					long *currentTriangle = nullptr;
					long currentIndex = startIndices[firstNeighbor];
					for (; currentIndex < currentEnd;)
					{
						for (; currentIndex < currentEnd && isActive[edgeList[currentIndex]]; ++currentIndex)
						{
						}
						for (; currentIndex < currentEnd && !isActive[edgeList[currentEnd - 1]]; --currentEnd)
						{
						}

						if (currentIndex >= currentEnd)
							break;
						std::swap(edgeList[currentIndex], edgeList[currentEnd - 1]);

						if (currentTriangle != nullptr)
						{
							std::swap(currentTriangle[currentIndex], currentTriangle[currentEnd - 1]);
						}
					}

					long temp = firstNeighbor;
					firstNeighbor = (edgeList[startIndices[firstNeighbor]] != prevVertex) ? edgeList[startIndices[firstNeighbor]] : edgeList[startIndices[firstNeighbor] + 1];
					prevVertex = temp;
				}

				if (firstNeighbor == vertexU)
				{
					long v = vertexU;
					isActive[v] = 0;

					for (int k = startIndices[v]; k < pending[v]; k++)
					{
						if (isActive[edgeList[k]])
						{
							long w = edgeList[k];
							--vertexDegree[w];
							if (vertexDegree[w] == 0)
								independentSetSize += 1;
							else if (vertexDegree[w] == 1)
								degreeOneVertices.push_back(w);
							else if (vertexDegree[w] == 2)
								degreeTwoVertices.push_back(w);
						}
					}

					continue;
				}

				prevVertex = vertexU;
				while (vertexDegree[secondNeighbor] == 2)
				{
					++chainLength;
					long currentVertex = secondNeighbor;
					long &currentEnd = pending[secondNeighbor];
					long *currentTriangle = nullptr;
					long currentIndex = startIndices[secondNeighbor];

					for (; currentIndex < currentEnd;)
					{
						for (; currentIndex < currentEnd && isActive[edgeList[currentIndex]]; ++currentIndex)
						{
						}
						for (; currentIndex < currentEnd and !isActive[edgeList[currentEnd - 1]]; --currentEnd)
						{
						}

						if (currentIndex >= currentEnd)
							break;
						std::swap(edgeList[currentIndex], edgeList[currentEnd - 1]);

						if (currentTriangle != nullptr)
						{
							std::swap(currentTriangle[currentIndex], currentTriangle[currentEnd - 1]);
						}
					}

					long temp = secondNeighbor;
					secondNeighbor = (edgeList[startIndices[secondNeighbor]] != prevVertex) ? edgeList[startIndices[secondNeighbor]] : edgeList[startIndices[secondNeighbor] + 1];
					prevVertex = temp;
				}
				if (firstNeighbor == secondNeighbor)
				{
					long v = firstNeighbor;
					isActive[v] = 0;

					for (int k = startIndices[v]; k < pending[v]; k++)
					{
						if (isActive[edgeList[k]])
						{
							long w = edgeList[k];
							--vertexDegree[w];
							if (vertexDegree[w] == 0)
								independentSetSize += 1;
							else if (vertexDegree[w] == 1)
								degreeOneVertices.push_back(w);
							else if (vertexDegree[w] == 2)
								degreeTwoVertices.push_back(w);
						}
					}

					continue;
				}

				long firstHalf = firstNeighbor;
				long &newEndFirst = pending[firstNeighbor];
				long *newTriangleFirst = nullptr;
				long iteratorFirst = startIndices[firstHalf];
				for (; iteratorFirst < newEndFirst;)
				{
					for (; iteratorFirst < newEndFirst && isActive[edgeList[iteratorFirst]]; ++iteratorFirst)
					{
					}
					for (; iteratorFirst < newEndFirst && !isActive[edgeList[newEndFirst - 1]]; --newEndFirst)
					{
					}

					if (iteratorFirst >= newEndFirst)
						break;

					std::swap(edgeList[iteratorFirst], edgeList[newEndFirst - 1]);

					if (newTriangleFirst != nullptr)
					{
						std::swap(newTriangleFirst[iteratorFirst], newTriangleFirst[newEndFirst - 1]);
					}
				}

				long secondHalf = secondNeighbor;
				long &newEndSecond = pending[secondNeighbor];
				long *newTriangleSecond = nullptr;
				long iteratorSecond = startIndices[secondHalf];
				for (; iteratorSecond < newEndSecond;)
				{
					for (; iteratorSecond < newEndSecond && isActive[edgeList[iteratorSecond]]; ++iteratorSecond)
					{
					}
					for (; iteratorSecond < newEndSecond && !isActive[edgeList[newEndSecond - 1]]; --newEndSecond)
					{
					}

					if (iteratorSecond >= newEndSecond)
						break;
					std::swap(edgeList[iteratorSecond], edgeList[newEndSecond - 1]);

					if (newTriangleSecond != nullptr)
					{
						std::swap(newTriangleSecond[iteratorSecond], newTriangleSecond[newEndSecond - 1]);
					}
				}

				if (chainLength % 2 == 1)
				{
					long v = firstHalf, newEdgesCount = 0;
					isActive[v] = 0;

					for (int edgeIndex = startIndices[v]; edgeIndex < pending[v]; edgeIndex++)
					{
						long adjacentVertex = edgeList[edgeIndex];

						if (isActive[adjacentVertex])
						{
							--vertexDegree[adjacentVertex];

							switch (vertexDegree[adjacentVertex])
							{
							case 0:
								++newEdgesCount;
								break;
							case 1:
								degreeOneVertices.push_back(adjacentVertex);
								break;
							case 2:
								degreeTwoVertices.push_back(adjacentVertex);
								break;
							}
						}
					}

					independentSetSize += newEdgesCount;

					long vv = secondHalf;
					isActive[vv] = 0;

					for (int k = startIndices[vv]; k < pending[vv]; k++)
					{
						if (isActive[edgeList[k]])
						{
							long w = edgeList[k];
							--vertexDegree[w];
							if (vertexDegree[w] == 0)
								independentSetSize += 1;
							else if (vertexDegree[w] == 1)
								degreeOneVertices.push_back(w);
							else if (vertexDegree[w] == 2)
								degreeTwoVertices.push_back(w);
						}
					}
				}
				else
				{
					long previousVertex = prevVertex, currentVertex = prevVertex;
					prevVertex = secondNeighbor;
					while (currentVertex != firstHalf)
					{
						isActive[currentVertex] = 0;
						long temp = currentVertex;
						currentVertex = (edgeList[startIndices[currentVertex]] == prevVertex) ? edgeList[startIndices[currentVertex] + 1] : edgeList[startIndices[currentVertex]];
						stack.push_back(std::make_pair(temp, currentVertex));
						prevVertex = temp;
					}
					currentVertex = prevVertex;

					long nodeU = firstHalf;
					long nodeV = secondHalf;
					int edgeFound = 0;
					long edgeIndexU = startIndices[nodeU];
					long endU = pending[nodeU];
					long endV = pending[nodeV];

					while (edgeIndexU < endU)
					{
						if (edgeList[edgeIndexU] == nodeV)
							edgeFound = 1;
						++edgeIndexU;
					}

					long edgeIndexV = startIndices[nodeV];
					while (edgeIndexV < endV)
					{
						if (edgeList[edgeIndexV] == nodeU)
							edgeFound = 1;
						++edgeIndexV;
					}
					if (edgeFound == 1)
					{
						--vertexDegree[firstHalf];
						--vertexDegree[secondHalf];
						if (vertexDegree[firstHalf] == 2)
							degreeTwoVertices.push_back(firstHalf);
						if (vertexDegree[secondHalf] == 2)
							degreeTwoVertices.push_back(secondHalf);
					}
					else
					{
						long vertexU = firstHalf;
						long target1 = currentVertex;
						long target2 = secondHalf;
						long edgePosU = startIndices[vertexU];
						long endPosU = pending[vertexU];

						for (; edgePosU < endPosU; ++edgePosU)
						{
							if (edgeList[edgePosU] == target1)
							{
								edgeList[edgePosU] = target2;
							}
						}

						long vertexV = secondHalf;
						long targetOpposite = previousVertex;
						long targetReplace = firstHalf;
						long edgePosV = startIndices[vertexV];
						long endPosV = pending[vertexV];

						for (; edgePosV < endPosV; ++edgePosV)
						{
							if (edgeList[edgePosV] == targetOpposite)
							{
								edgeList[edgePosV] = targetReplace;
							}
						}
					}
				}
			}
		}

		for (; degreeOneVertices.empty() && degreeTwoVertices.empty();)
		{
			for (; maxDegree >= 3 && degreeLL[maxDegree] == -1; --maxDegree)
				;

			if (maxDegree < 3)
				break;

			int highestDegreeVertex = -1;
			for (highestDegreeVertex = degreeLL[maxDegree]; highestDegreeVertex != -1;)
			{
				int nextInBin = degreeLLChain[highestDegreeVertex];
				if (isActive[highestDegreeVertex] && vertexDegree[highestDegreeVertex] > 0)
				{
					if (vertexDegree[highestDegreeVertex] < maxDegree)
					{
						degreeLLChain[highestDegreeVertex] = degreeLL[vertexDegree[highestDegreeVertex]];
						degreeLL[vertexDegree[highestDegreeVertex]] = highestDegreeVertex;
					}
					else
					{
						stack.push_back(std::make_pair(highestDegreeVertex, numVertices));

						isActive[highestDegreeVertex] = 0;

						int edgeIter = startIndices[highestDegreeVertex];
						while (edgeIter < pending[highestDegreeVertex])
						{
							bool isActiveEdge = isActive[edgeList[edgeIter]];
							if (isActiveEdge)
							{
								int adjacentVertex = edgeList[edgeIter];
								--vertexDegree[adjacentVertex];

								bool hasNoRemainingConnections = (vertexDegree[adjacentVertex] == 0);
								if (hasNoRemainingConnections)
								{
									independentSetSize += 1;
								}
								else
								{
									bool isDegreeOne = (vertexDegree[adjacentVertex] == 1);
									if (isDegreeOne)
									{
										degreeOneVertices.push_back(adjacentVertex);
									}
									else
									{
										bool isDegreeTwo = (vertexDegree[adjacentVertex] == 2);
										if (isDegreeTwo)
										{
											degreeTwoVertices.push_back(adjacentVertex);
										}
									}
								}
							}
							edgeIter++;
						}

						degreeLL[maxDegree] = nextInBin;
						break;
					}
				}
				highestDegreeVertex = nextInBin;
			}
			if (highestDegreeVertex == -1)
				degreeLL[maxDegree] = -1;
		}
	}

	for (long i = stack.size() - 1; i >= 0; i--)
	{
		long vertex1 = stack[i].first, vertex2 = stack[i].second;
		if (vertex2 != numVertices)
		{
			if (!isActive[vertex2])
			{
				isActive[vertex1] = 1;
				independentSetSize += 1;
			}
			continue;
		}

		int isValid = 1;
		for (long j = startIndices[vertex1]; j < startIndices[vertex1 + 1]; j++)
			if (isActive[edgeList[j]])
			{
				isValid = 0;
				break;
			}
		if (isValid)
		{
			isActive[vertex1] = 1;
			independentSetSize += 1;
		}
	}

	return independentSetSize;
}

int greedyAlgorithm(long vertices, long edgesCount, std::vector<long> &startIndices, std::vector<long> &edges)
{
	std::vector<char> isVertexAvailable(vertices, 1);
	std::vector<long> degreeIds(vertices, 0);
	std::vector<long> degreeOrder(vertices);

	for (long index = 0; index < vertices; index++)
	{
		long degree = startIndices[index + 1] - startIndices[index];
		++degreeIds[degree];
	}

	for (long i = 1; i < vertices; i++)
		degreeIds[i] += degreeIds[i - 1];

	for (long index = vertices - 1; index >= 0; index--)
	{
		long degree = startIndices[index + 1] - startIndices[index];
		degreeOrder[--degreeIds[degree]] = index;
	}

	int independentSetSize = 0;
	for (long index = 0; index < vertices; index++)
	{
		if (isVertexAvailable[degreeOrder[index]])
		{
			long u = degreeOrder[index];
			++independentSetSize;
			for (long j = startIndices[u]; j < startIndices[u + 1]; j++)
				isVertexAvailable[edges[j]] = 0;
		}
	}

	return independentSetSize;
}

int main(int argc, char *argv[])
{
	long vertices = 0;
	long edgesCount = 0;
	long *startIndices;
	long *edges;

	FILE *degreeFile = fopen((std::string(argv[1]) + "/degree.dat").c_str(), "rb");
	FILE *adjFile = fopen((std::string(argv[1]) + "/adjacency.dat").c_str(), "rb");

	int type;
	if (fread(&type, sizeof(int), 1, degreeFile) == 1)
	{
		fread(&vertices, sizeof(int), 1, degreeFile);
		fread(&edgesCount, sizeof(int), 1, degreeFile);

		std::vector<long> nodeDegrees(vertices);
		fread(nodeDegrees.data(), sizeof(int), vertices, degreeFile);
		fclose(degreeFile);

		startIndices = new long[vertices + 1];
		edges = new long[edgesCount];

		startIndices[0] = 0;
		std::vector<long> buffer(vertices);

		long i = 0;
		while (i < vertices)
		{
			long j = 0;
			while (j < nodeDegrees[i])
			{
				if (nodeDegrees[i] > 0 && fread(buffer.data(), sizeof(int), nodeDegrees[i] - j, adjFile) > 0)
				{
					for (long k = 0; k < nodeDegrees[i] - j; k++)
					{
						edges[startIndices[i] + j + k] = buffer[k];
					}
					j += nodeDegrees[i] - j;
				}
				else
				{
					break;
				}
			}
			startIndices[i + 1] = startIndices[i] + nodeDegrees[i];
			i++;
		}
		fclose(adjFile);
	}

	std::vector<long> startIndicesVec(startIndices, startIndices + vertices + 1);
	std::vector<long> edgesVec(edges, edges + edgesCount);

	std::cout << "Choose an algorithm:\n"
			  << "1. BDOne\n"
			  << "2. BDTwo\n"
			  << "3. LinearTime\n"
			  << "4. Greedy\n"
			  << "Enter the number of the algorithm: ";

	int choice;
	std::cin >> choice;
	int maximumIndependentSet = 0;

	auto t1 = std::chrono::high_resolution_clock::now();

	switch (choice)
	{
	case 1:
		maximumIndependentSet = bdOneAlgorithm(vertices, edgesCount, startIndicesVec, edgesVec);
		break;
	case 2:
		maximumIndependentSet = bdTwoAlgorithm(vertices, edgesCount, startIndicesVec, edgesVec);
		break;
	case 3:
		maximumIndependentSet = linearTimeAlgorithm(vertices, edgesCount, startIndicesVec, edgesVec);
		break;
	case 4:
		maximumIndependentSet = greedyAlgorithm(vertices, edgesCount, startIndicesVec, edgesVec);
		break;
	default:
		std::cout << "Invalid choice.\n";
		exit(1);
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> durationRun = t2 - t1;

	std::cout << "\nNumber of nodes = " << vertices;
	std::cout << "\nNumber of edges = " << (int)edgesCount / 2;
	std::cout << "\nMaximum Independent Set Size = " << maximumIndependentSet;
	std::cout << "\nDuration = " << durationRun.count() << "ms\n";

	return 0;
}
