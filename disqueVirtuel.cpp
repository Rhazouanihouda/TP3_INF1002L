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
// vous pouvez inclure d'autres librairies si c'est nécessaire

namespace TP3
{

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
				m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i] = false;
			}

			// Marquez le i-nodes 0 comme occupé - contient la liste des blocs defectueux.
			m_blockDisque[FREE_INODE_BITMAP].m_bitmap[0] = false;

			// Initialize les i-nodes dans les blocs 4 à 23.
			for (int i = BASE_BLOCK_INODE; i < 24; i++)
			{
				m_blockDisque[i].m_inode = iNode(i - BASE_BLOCK_INODE, 0, 0, 0, i);
				m_blockDisque[i].m_type_donnees = S_IFIN; // Initializer le type de données du bloc.
			}

			// Initializer le i-node 1 du répertoire racine.
			m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_inode->st_mode = S_IFDIR;
			// m_blockDisque[BASE_BLOCK_INODE + ROOT_INODE].m_dirEntry = std::vector<dirEntry *>(new dirEntry(1, "/")); // Info sur le repertoire racine.
			//  Marquez le i-node 1 comme occupé - Pour le repertoire racine.
			m_blockDisque[FREE_INODE_BITMAP].m_bitmap[1] = false;

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
	}

	int DisqueVirtuel::bd_mkdir(const std::string &p_DirName)
	{
		// TODO
	}

	std::string DisqueVirtuel::bd_ls(const std::string &p_DirLocation)
	{
		// TODO
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
} // Fin du namespace
