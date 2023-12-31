/**
 * \file disqueVirtuel.cpp
 * \brief Implémentation d'un disque virtuel.
 * \author ?
 * \version 0.1
 * \date  2023
 *
 *  Travail pratique numéro 3
 *
 */

#include "disqueVirtuel.h"
#include <iostream>
#include <string>
#include <sstream>
// vous pouvez inclure d'autres librairies si c'est nécessaire

namespace TP3
{

#pragma region public methods
	DisqueVirtuel::DisqueVirtuel()
	{
	}
	DisqueVirtuel::~DisqueVirtuel()
	{
	}
	// Ajouter votre code ici !
	int DisqueVirtuel::bd_FormatDisk()
	{
		try
		{
			// TODO
			m_blockDisque = std::vector<Block>(N_BLOCK_ON_DISK); // Initializer le disque.

			// Initializer le bitmap des blocs libres.
			m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap = std::vector<bool>(N_BLOCK_ON_DISK, true);
			m_blockDisque[FREE_BLOCK_BITMAP].m_type_donnees = S_IFBL; // Initializer le type de données du bloc
			// Initializer le bitmap des i-nodes libres.
			m_blockDisque[FREE_INODE_BITMAP].m_bitmap = std::vector<bool>(N_INODE_ON_DISK, true);
			m_blockDisque[FREE_INODE_BITMAP].m_type_donnees = S_IFIL; // Initializer le type de données du bloc.

			// Marquez les blocs 0 à 23 comme occupés.
			for (int i = 0; i < 24; i++)
			{
				mark_block_as_used(i, false);
			}

			// Marquez le i-nodes 0 comme occupé - contient la liste des blocs defectueux.
			m_blockDisque[FREE_INODE_BITMAP].m_bitmap[0] = false;

			// Initialize les i-nodes dans les blocs 4 à 23.
			for (int i = BASE_BLOCK_INODE; i < 24; i++)
			{
				m_blockDisque[i].m_inode = new iNode(i - BASE_BLOCK_INODE, 0, 0, 0, i);
				m_blockDisque[i].m_type_donnees = S_IFIN; // Initializer le type de données du bloc.
			}

			// Initializer le i-node 1 du répertoire racine.
			m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_inode->st_mode = S_IFDIR;
			m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_dirEntry = std::vector<dirEntry *>();
			create_dot_entry(ROOT_INODE);
			create_dot_dot_entry(ROOT_INODE, ROOT_INODE);

			//  Marquez le i-node 1 comme occupé - Pour le repertoire racine.
			mark_i_node_as_used(ROOT_INODE, false);

			return 1;
		}
		catch (...)
		{
			return 0;
		}
	}

	int DisqueVirtuel::bd_create(const std::string &p_FileName)
	{
		// TODO
		auto splitResult = split(p_FileName, '/');
		auto fileName = splitResult[splitResult.size() - 1];

		size_t dNode_block_number = BASE_BLOCK_INODE + ROOT_INODE;
		// Loop over the split result and find the i-node of the last directory.
		for (int i = 0; i < splitResult.size(); i++)
		{
			if (splitResult[i] == fileName)
			{

				// Check if file already exists...
				if (file_exists_on_block(dNode_block_number, fileName))
				{
					return 0;
				}

				// Find the first free i-node and create the new file.
				auto n = bd_find_first_free_i_node();
				m_blockDisque[n + BASE_BLOCK_INODE].m_inode = new iNode(n, S_IFREG, 1, 0, n + BASE_BLOCK_INODE);
				m_blockDisque[n + BASE_BLOCK_INODE].m_type_donnees = S_IFIN; // Initializer le type de données du bloc.
				mark_block_as_used(n + BASE_BLOCK_INODE, false);
				mark_i_node_as_used(n, false);
				m_blockDisque[dNode_block_number].m_dirEntry.push_back(new dirEntry(n, fileName));
				return 1;
			}
			else
			{
				// Find the i-node of the directory.
				for (int j = 0; j < m_blockDisque[dNode_block_number].m_dirEntry.size(); j++)
				{
					if (m_blockDisque[dNode_block_number].m_dirEntry[j]->m_filename == splitResult[i])
					{
						dNode_block_number = BASE_BLOCK_INODE + m_blockDisque[dNode_block_number].m_dirEntry[j]->m_iNode;
					}
				}
			}
		}
		return 0;
	}

	int DisqueVirtuel::bd_mkdir(const std::string &p_DirName)
	{
		// TODO
		auto splitResult = split(p_DirName, '/');
		if (splitResult.size() > 1)
		{
			auto folderName = splitResult[splitResult.size() - 1];

			size_t dNode_block_number = BASE_BLOCK_INODE + ROOT_INODE;
			// Loop over the split result and find the i-node of the last directory.
			for (int i = 0; i < splitResult.size(); i++)
			{
				if (splitResult[i] == folderName)
				{

					// Check if file already exists...
					if (folder_exists_on_block(dNode_block_number, folderName))
					{
						return 0;
					}

					// Find the first free i-node and create the new folder.
					auto n = bd_find_first_free_i_node();
					m_blockDisque[n + BASE_BLOCK_INODE].m_inode = new iNode(n, S_IFDIR, 1, 0, n + BASE_BLOCK_INODE);
					m_blockDisque[n + BASE_BLOCK_INODE].m_dirEntry = std::vector<dirEntry *>(); // Initializer le type de données du bloc.
					m_blockDisque[n + BASE_BLOCK_INODE].m_type_donnees = S_IFIN;				// Initializer le type de données du bloc.
					mark_block_as_used(n + BASE_BLOCK_INODE, false);
					mark_i_node_as_used(n, false);
					create_dot_entry(n);
					create_dot_dot_entry(n, dNode_block_number - BASE_BLOCK_INODE);
					m_blockDisque[dNode_block_number].m_dirEntry.push_back(new dirEntry(n, folderName));
					m_blockDisque[dNode_block_number].m_inode->st_nlink++;
					return 1;
				}
				else
				{
					// Find the i-node of the directory.
					for (int j = 0; j < m_blockDisque[dNode_block_number].m_dirEntry.size(); j++)
					{
						if (m_blockDisque[dNode_block_number].m_dirEntry[j]->m_filename == splitResult[i])
						{
							dNode_block_number = BASE_BLOCK_INODE + m_blockDisque[dNode_block_number].m_dirEntry[j]->m_iNode;
						}
					}
				}
			}
		}
		else
		{
			// Find the first free i-node and create the new folder.
			auto n = bd_find_first_free_i_node();
			m_blockDisque[n + BASE_BLOCK_INODE].m_inode = new iNode(n, S_IFDIR, 1, 0, n + BASE_BLOCK_INODE);
			m_blockDisque[n + BASE_BLOCK_INODE].m_dirEntry = std::vector<dirEntry *>(); // Initializer le type de données du bloc.
			m_blockDisque[n + BASE_BLOCK_INODE].m_type_donnees = S_IFIN;				// Initializer le type de données du bloc.
			mark_block_as_used(n + BASE_BLOCK_INODE, false);
			mark_i_node_as_used(n, false);
			create_dot_entry(n);
			create_dot_dot_entry(n, n);
			m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_dirEntry.push_back(new dirEntry(n, p_DirName));
			m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_inode->st_nlink++;
			return 1;
		}

		return 0;
	}

	std::string DisqueVirtuel::bd_ls(const std::string &p_DirLocation)
	{
		// TODO
		std::stringstream ss;
		auto splitResult = split(p_DirLocation, '/');

		if (splitResult.size() > 1)
		{
			auto folderName = splitResult[splitResult.size() - 1];
			size_t dNode_block_number = BASE_BLOCK_INODE + ROOT_INODE;
			for (int i = 0; i < splitResult.size(); i++)
			{
				if (splitResult[i] == folderName)
				{
					// Check if folder already exists...
					if (!folder_exists_on_block(dNode_block_number, folderName))
					{
						ss << "Directory " << p_DirLocation << " does not exist." << std::endl;
						return ss.str();
					}
					else
					{
						for (int j = 0; j < m_blockDisque[dNode_block_number].m_dirEntry.size(); j++)
						{
							if (m_blockDisque[dNode_block_number].m_dirEntry[j]->m_filename == folderName &&
								m_blockDisque[m_blockDisque[dNode_block_number].m_dirEntry[j]->m_iNode + BASE_BLOCK_INODE].m_inode->st_mode == S_IFDIR)
							{
								dNode_block_number = BASE_BLOCK_INODE + m_blockDisque[dNode_block_number].m_dirEntry[j]->m_iNode;
								break;
							}
						}
						for (int k = 0; k < m_blockDisque[dNode_block_number].m_dirEntry.size(); k++)
						{
							m_blockDisque[BASE_BLOCK_INODE + m_blockDisque[dNode_block_number].m_dirEntry[k]->m_iNode].m_inode->st_mode == S_IFDIR
								? ss << "d\t"
								: ss << "-\t";
							ss << m_blockDisque[dNode_block_number].m_dirEntry[k]->m_filename
							   << " Size: \t"
							   << " inode: \t"
							   << " nlink: \t" << std::endl;
						}
						return ss.str();
					}
				}
				else
				{
					// Find the i-node of the directory.
					for (int j = 0; j < m_blockDisque[dNode_block_number].m_dirEntry.size(); j++)
					{
						if (m_blockDisque[dNode_block_number].m_dirEntry[j]->m_filename == splitResult[i])
						{
							dNode_block_number = BASE_BLOCK_INODE + m_blockDisque[dNode_block_number].m_dirEntry[j]->m_iNode;
						}
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_dirEntry.size(); i++)
			{
				m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_inode->st_mode == S_IFDIR
					? ss << "d\t"
					: ss << "-\t";
				ss << m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_dirEntry[i]->m_filename
				   << "\t Size: \t"
				   << "\t inode: \t"
				   << "\t nlink: \t" << std::endl;
			}
		}

		return ss.str();
	}

	int DisqueVirtuel::bd_rm(const std::string &p_Filename)
	{
		// TODO
	}

	int DisqueVirtuel::bd_find_first_free_i_node()
	{
		// TODO
		for (int i = 0; i < m_blockDisque[FREE_INODE_BITMAP].m_bitmap.size(); i++)
		{
			if (m_blockDisque[FREE_INODE_BITMAP].m_bitmap[i] == true)
			{
				return i;
			}
		}
	}

	int DisqueVirtuel::bd_find_first_free_block()
	{
		// TODO
		for (int i = 0; i < m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap.size(); i++)
		{
			if (m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i] == true)
			{
				return i;
			}
		}
	}

	int DisqueVirtuel::mark_i_node_as_used(const size_t &p_iNodeNumber, const bool &p_isUsed)
	{
		m_blockDisque[FREE_INODE_BITMAP].m_bitmap[p_iNodeNumber] = p_isUsed;
	}

	int DisqueVirtuel::mark_block_as_used(const size_t &p_BlockNumber, const bool &p_isUsed)
	{
		m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[p_BlockNumber] = p_isUsed;
	}

	bool DisqueVirtuel::file_exists_on_block(const size_t &p_blockNumber, const std::string &p_FileName)
	{
		for (int k = 0; k < m_blockDisque[p_blockNumber].m_dirEntry.size(); k++)
		{
			if (m_blockDisque[p_blockNumber].m_dirEntry[k]->m_filename == p_FileName &&
				m_blockDisque[m_blockDisque[p_blockNumber].m_dirEntry[k]->m_iNode + BASE_BLOCK_INODE].m_inode->st_mode == S_IFREG)
			{
				return true;
			}
		}
		return false;
	}

	bool DisqueVirtuel::folder_exists_on_block(const size_t &p_blockNumber, const std::string &p_FolderName)
	{
		for (int k = 0; k < m_blockDisque[p_blockNumber].m_dirEntry.size(); k++)
		{
			if (m_blockDisque[p_blockNumber].m_dirEntry[k]->m_filename == p_FolderName &&
				m_blockDisque[m_blockDisque[p_blockNumber].m_dirEntry[k]->m_iNode + BASE_BLOCK_INODE].m_inode->st_mode == S_IFDIR)
			{
				std::cout << "Found folder " << p_FolderName << std::endl;
				return true;
			}
		}
		return false;
	}

	bool DisqueVirtuel::create_dot_entry(const size_t &p_iNodeNumber)
	{
		try
		{
			m_blockDisque[BASE_BLOCK_INODE + p_iNodeNumber].m_dirEntry.push_back(new dirEntry(p_iNodeNumber, "."));
			m_blockDisque[BASE_BLOCK_INODE + p_iNodeNumber].m_inode->st_nlink++;
			return true;
		}
		catch (...)
		{
			return false;
		}
		return false;
	}

	bool DisqueVirtuel::create_dot_dot_entry(const size_t &p_iNodeNumber, const size_t &p_parentINodeNumber)
	{
		try
		{
			m_blockDisque[BASE_BLOCK_INODE + p_iNodeNumber].m_dirEntry.push_back(new dirEntry(p_parentINodeNumber, ".."));
			if (p_iNodeNumber == p_parentINodeNumber)
			{
				m_blockDisque[BASE_BLOCK_INODE + p_parentINodeNumber].m_inode->st_nlink++;
			}
			else
			{
				m_blockDisque[BASE_BLOCK_INODE + p_parentINodeNumber].m_inode->st_nlink++;
				m_blockDisque[BASE_BLOCK_INODE + p_iNodeNumber].m_inode->st_nlink++;
			}
			return true;
		}
		catch (...)
		{
			return false;
		}
		return false;
	}

#pragma endregion

#pragma region private methods

	std::vector<std::string> DisqueVirtuel::split(const std::string &s, char delim)
	{
		std::vector<std::string> result = std::vector<std::string>();
		std::stringstream ss(s);
		std::string item;
		while (getline(ss, item, delim))
		{
			result.push_back(item);
		}
		return result;
	}
#pragma endregion
} // Fin du namespace
