#include "ext2fs.h"
#include "hdd.h"
#include "pentry.h"
#include "mbr.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <algorithm> // Para std::min


Ext2FS::Ext2FS(HDD & disk, unsigned char pnumber) : _hdd(disk), _partition_number(pnumber)
{
	assert(pnumber <= 3);

	// Load Superblock
	unsigned char * buffer = new unsigned char[2*SECTOR_SIZE];

	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Skip first two sectors & read 3rd and 4th sectors
	// Warning: We assume common sector size of 512bytes but it may not be true
	_hdd.read(pentry.start_lba()+2, buffer);
	_hdd.read(pentry.start_lba()+3, buffer+SECTOR_SIZE);

	// Warning: this only works in Little Endian arhitectures
	_superblock = (struct Ext2FSSuperblock *) buffer;

	// Load Block Group Table

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	// Compute amount of block groups in FS
	_block_groups = _superblock->blocks_count / _superblock->blocks_per_group;

	if((_superblock->blocks_count % _superblock->blocks_per_group) != 0)
	{
		_block_groups++;
	}
	std::cerr << "block_groups: " << _block_groups << std::endl;

	// Allocate enough memory to keep the table
	_bgd_table = new Ext2FSBlockGroupDescriptor[_block_groups];

	// Since the size of the table is not forced to be a factor of SECTOR_SIZE
	// we need to compute the amount of sectors we should read and then copy
	// one by one the descriptors into the table in the apropriate order.
	unsigned int size_of_bgd_table = sizeof(Ext2FSBlockGroupDescriptor) * _block_groups;
	unsigned int bgd_table_sectors = size_of_bgd_table / SECTOR_SIZE;
	if((size_of_bgd_table % SECTOR_SIZE) != 0)
	{
		bgd_table_sectors++;
	}
	std::cerr << "bgd_table_sectors: " << bgd_table_sectors << std::endl;

	buffer = new unsigned char[bgd_table_sectors * SECTOR_SIZE];
	for(unsigned int i = 0; i < bgd_table_sectors; i++)
	{
		std::cerr << "pentry.start_lba()+4+i: " << (unsigned int) (pentry.start_lba()+4+i) << " buffer+(SECTOR_SIZE*i): " << (unsigned char *) (buffer+(SECTOR_SIZE*i)) << std::endl;
		_hdd.read(pentry.start_lba()+4+i, buffer+(SECTOR_SIZE*i));
	}

	// Copy descriptors into table
	for(unsigned int i = 0; i < _block_groups; i++)
	{
		_bgd_table[i] = *((Ext2FSBlockGroupDescriptor *) (buffer+(i*sizeof(Ext2FSBlockGroupDescriptor))));
	}
	delete[] buffer;
}

unsigned int Ext2FS::blockgroup_for_inode(unsigned int inode)
{
	assert(inode > 0);
	return (inode - 1) / _superblock->inodes_per_group;
}

unsigned int Ext2FS::blockgroup_inode_index(unsigned int inode)
{
	assert(inode > 0);
	return (inode - 1) % _superblock->inodes_per_group;
}

// Free allocated memory
Ext2FS::~Ext2FS()
{
	delete[] _superblock;
	delete[] _bgd_table;
}

struct Ext2FSSuperblock * Ext2FS::superblock()
{
	return _superblock;
}

unsigned int Ext2FS::block_groups()
{
	return _block_groups;
}

struct Ext2FSBlockGroupDescriptor * Ext2FS::block_group(unsigned int index)
{
	assert(index < _block_groups);
	return & _bgd_table[index];
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSInode & inode)
{
	output << "mode: " << inode.mode << std::endl;
	output << "uid: " << inode.uid << std::endl;
	output << "size: " << inode.size << std::endl;
	output << "atime: " << inode.atime << std::endl;
	output << "ctime: " << inode.ctime << std::endl;
	output << "mtime: " << inode.mtime << std::endl;
	output << "dtime: " << inode.dtime << std::endl;
	output << "gid: " << inode.gid << std::endl;
	output << "links_count: " << inode.links_count << std::endl;
	output << "blocks: " << inode.blocks << std::endl;
	output << "flags: " << inode.flags << std::endl;
	//output << "os_dependant_1: " << inode.os_dependant_1 << std::endl;

	//output << std::hex;
	for(unsigned int i = 0; i < 15; i++)
		output << "block[" << i << "]: " << inode.block[i] << std::endl;
	//output << std:dec;

	output << "generation: " << inode.generation << std::endl;
	output << "file_acl: " << inode.file_acl << std::endl;
	output << "directory_acl: " << inode.directory_acl << std::endl;
	output << "faddr: " << inode.faddr << std::endl;
	//output << "os_dependant_2: " << inode.os_dependant_2 << std::endl[3];

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSSuperblock & superblock)
{
	output << "inodes_count: " << superblock.inodes_count << std::endl;
	output << "blocks_count: " << superblock.blocks_count << std::endl;
	output << "reserved_blocks_count: " << superblock.reserved_blocks_count << std::endl;
	output << "free_blocks_count: " << superblock.free_blocks_count << std::endl;
	output << "free_inodes_count: " << superblock.free_inodes_count << std::endl;
	output << "first_data_block: " << superblock.first_data_block << std::endl;
	output << "log_block_size: " << superblock.log_block_size << std::endl;
	output << "log_fragment_size: " << superblock.log_fragment_size << std::endl;
	output << "blocks_per_group: " << superblock.blocks_per_group << std::endl;
	output << "fragments_per_group: " << superblock.fragments_per_group << std::endl;
	output << "inodes_per_group: " << superblock.inodes_per_group << std::endl;
	output << "mount_time: " << superblock.mount_time << std::endl;
	output << "write_time: " << superblock.write_time << std::endl;
	output << "mount_count: " << superblock.mount_count << std::endl;
	output << "max_mount_count: " << superblock.max_mount_count << std::endl;
	output << "magic_bytes: " << superblock.magic_bytes << std::endl;
	output << "state: " << superblock.state << std::endl;
	output << "errors: " << superblock.errors << std::endl;
	output << "minor_revision_level: " << superblock.minor_revision_level << std::endl;
	output << "lastcheck: " << superblock.lastcheck << std::endl;
	output << "checkinterval: " << superblock.checkinterval << std::endl;
	output << "creator_os: " << superblock.creator_os << std::endl;
	output << "revision_level: " << superblock.revision_level << std::endl;
	output << "default_reserved_userid: " << superblock.default_reserved_userid << std::endl;
	output << "default_reserved_groupid: " << superblock.default_reserved_groupid << std::endl;
	// -- EXT2_DYNAMIC_REV Specific --
	output << "first_inode: " << superblock.first_inode << std::endl;
	output << "inode_size: " << superblock.inode_size << std::endl;
	output << "block_group_number: " << superblock.block_group_number << std::endl;
	output << "feature_compatibility: " << superblock.feature_compatibility << std::endl;
	output << "feature_incompatibility: " << superblock.feature_incompatibility << std::endl;
	output << "readonly_feature_compatibility: " << superblock.readonly_feature_compatibility << std::endl;
	output << "uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.uuid[i];
	output << std::endl;
	output << "volume_name: ";
	for(int i = 0; i < 16; i++)
		output << superblock.volume_name[i];
	output << std::endl;
	output << "last_mounted: ";
	for(int i = 0; i < 64; i++)
		output << superblock.last_mounted[i];
	output << std::endl;
	output << "algo_bitmap: " << superblock.algo_bitmap << std::endl;
	// Performance hints
	output << "prealloc_blocks: " << (unsigned int) superblock.prealloc_blocks << std::endl;
	output << "prealloc_dir_blocks: " << (unsigned int) superblock.prealloc_dir_blocks << std::endl;
	//char alignment[2];
	// Journaling support
	output << "journal_uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.journal_uuid[i];
	output << std::endl;
	output << "journal_inode: " << superblock.journal_inode << std::endl;
	output << "journal_device: " << superblock.journal_device << std::endl;
	output << "last_orphan: " << superblock.last_orphan << std::endl;
	// Directory indexing support
	output << "hash_seed: ";
	for(int i = 0; i < 4; i++)
		output << superblock.hash_seed[i];
	output << std::endl;
	output << "default_hash_version: " << (unsigned int) superblock.default_hash_version << std::endl;
	//char padding[3];
	// Other options
	output << "default_mount_options: " << superblock.default_mount_options << std::endl;
	output << "first_meta_bg: " << superblock.first_meta_bg << std::endl;
	//char unused[760];

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSBlockGroupDescriptor & bg_descriptor)
{
	output << "block_bitmap: " << bg_descriptor.block_bitmap << std::endl;
	output << "inode_bitmap: " << bg_descriptor.inode_bitmap << std::endl;
	output << "inode_table: " << bg_descriptor.inode_table << std::endl;
	output << "free_blocks_count: " << bg_descriptor.free_blocks_count << std::endl;
	output << "free_inodes_count: " << bg_descriptor.free_inodes_count << std::endl;
	output << "used_dirs_count: " << bg_descriptor.used_dirs_count << std::endl;

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSDirEntry & dent)
{
	output << "inode: " << dent.inode << std::endl;
	output << "record_length: " << dent.record_length << std::endl;
	output << "name_length: " << (unsigned short) dent.name_length << std::endl;
	output << "file_type: " << (unsigned short) dent.file_type << std::endl;

	// NULL terminate dent name
	char * dentname = new char[dent.name_length + 1];
	strncpy(dentname, dent.name, dent.name_length);
	dentname[dent.name_length] = '\0';

	output << "name: " << dentname << std::endl;

	delete[] dentname;

	return output;
}

struct Ext2FSInode * Ext2FS::inode_for_path(const char * path)
{
	char * mypath = new char[strlen(path)+1];
	mypath[strlen(path)] = '\0';
	strncpy(mypath, path, strlen(path));

	char * pathtok = strtok(mypath, PATH_DELIM);
	struct Ext2FSInode * inode = NULL;

	while(pathtok != NULL)
	{
		struct Ext2FSInode * prev_inode = inode;
		// std::cerr << "pathtok: " << pathtok << std::endl;
		inode = get_file_inode_from_dir_inode(prev_inode, pathtok);
		pathtok = strtok(NULL, PATH_DELIM);

		delete prev_inode;
	}

	delete[] mypath;
	return inode;
}

unsigned int Ext2FS::blockaddr2sector(unsigned int block)
{
	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	return pentry.start_lba() + block * sectors_per_block;
}

/**
 * Warning: This method allocates memory that must be freed by the caller
 */
struct Ext2FSInode * Ext2FS::load_inode(unsigned int inode_number)
{
	//Traigo el supebloque
	Ext2FSSuperblock* super = superblock();


	//calculo cual blockgrup le corresponde
	unsigned int block_group_index = blockgroup_for_inode(inode_number);


	//cargo el block group
	Ext2FSBlockGroupDescriptor* block_group_descriptor = block_group(block_group_index);


	//calculo el block size (magia)
	unsigned int block_size = 1024 << super->log_block_size;


	//index dentro de la tabla de inodos , se hacehaciedo el modulo
	unsigned int inode_table_index = blockgroup_inode_index(inode_number);


	//el tamaño del bloque /el tamaño de los inodos, nos dice cuantos inodos en bloque ADENTRO DE LA INDEX TABLE
	unsigned int inodes_per_block = block_size / super->inode_size;

	//calculo el offset, otra magia, usando el index / la cantidad de inodos en un BLOQUE ADENTRO DE LA INDEX TABLE
	unsigned int offset = inode_table_index / inodes_per_block;



	//me armo un buffer para traer el bloque entero
	unsigned char * buffer = (unsigned char *) malloc(block_size);


	//Leo la table + el offset,asi me traigo el bloque que tiene mielemento
	read_block(block_group_descriptor->inode_table + offset, buffer);

	//inicializo memoria de manera tal que tenga dodne guardarme elinodo
	Ext2FSInode* inode = (Ext2FSInode*) malloc(sizeof(Ext2FSInode));

	// aca calculoel indice dentro del bloque, que por alguna razon magica, coincide con el index % inodes per block
	unsigned int inode_table_index_block_offset = inode_table_index % inodes_per_block;

	//hago una copia de memroia desde el offset, asi dentro del block me traigo justo el inodoque espero
	//Esto lo puedo hacer porque tengo un inodo de tamaño fijo
	std::memcpy(inode,(buffer + inode_table_index_block_offset * sizeof(Ext2FSInode)),sizeof(Ext2FSInode));

	return inode;
}

unsigned int Ext2FS::get_block_address(struct Ext2FSInode * inode, unsigned int block_number)
{
	//tamaño del bloque
	unsigned int block_size = 1024 << _superblock->log_block_size;

	//cantidad de punteros por bloque
	unsigned int addresses_per_block = block_size / sizeof(uint);

	if(block_number < 12) {
		return inode->block[block_number];
	}

	block_number-=12;
	//
	if (block_number <= addresses_per_block) {
	//soy el primer indirecto
	//me armo un buffer para traer el bloque entero
		unsigned char * buffer = (unsigned char *) malloc(block_size);
		//leo el bloque
		read_block(inode->block[12], buffer);
		//interpreto el bloque como un vector de uints
		uint * bloque_de_punteros = (uint *) buffer;

		uint addr = 0;

        // copiamos desde el buffer + block_numer el valor del adresss, creo que con bloque_de_punteros[block_number] funca?
        memcpy(&addr, buffer + block_number * sizeof(uint), sizeof(uint));

		return addr;
	}
	//volvemos a restar, porque significa que estamos en el segundo indirecto (minimo)
	//el bloque al que apunta es un bloque de block adress, que apuntan a otras block adress
	//basicamente tengo en cada adress, addresses_per_block veces de punteros
	//asi que si el block_number esta en el segundo indirecto, es < addresses_per_block * addresses_per_block
	block_number -=addresses_per_block;
	//std::cout << "Hasta cuando llega el segundo indirecto: " << addresses_per_block * addresses_per_block << std::endl;
	if(block_number < addresses_per_block * addresses_per_block) {
		//como antes, preparo un buffer para el bloque entero

		unsigned char * buffer = (unsigned char *) malloc(block_size);
		//traigo el bloque del segundo indirecto, que es el de doble indireccion no? el dibujito esta distinto
		read_block(inode->block[13], buffer);
		//aca podemos hacer algo a lo load inode no?
		//para saber el indice, es el numero del bloque / la cantidad de adresses por bloque
		//ponele que el numero sea 4, / 10 = 0 ok, 11 / 10 = 1 OK, 24 / 10, 2 OK division entera

		uint indice_tabla_de_tablas = block_number / addresses_per_block;

		uint adress_tabla_de_tablas = 0;
		memcpy(&adress_tabla_de_tablas, buffer + indice_tabla_de_tablas * sizeof(uint), sizeof(uint));

		//aca ya medio que ganamos, porque ya estamos en la tabla final, la que tiene adress a bloques o cosas
		//de verdad, es copiar y pegar lo de arriba

		unsigned char * bufferInterno = (unsigned char *) malloc(block_size);
		//leo el bloque
		read_block(adress_tabla_de_tablas, bufferInterno);
		//interpreto el bloque como un vector de uints
		uint * bloque_de_punteros = (uint *) bufferInterno;

		uint addr = 0;
		//porque esto funciona? lo mismo que en load_inode
		unsigned int adress_en_la_tabla = block_number % addresses_per_block;


        // copiamos desde el bufferInterno + el adress en la tabla, creo que con bloque_de_punteros[block_number] funca?
        memcpy(&addr, bufferInterno + adress_en_la_tabla * sizeof(uint), sizeof(uint));

		return addr;

		//TODO: preguntar, porque funciona asi?
		//que el numero / la cantidad de nodos/adress por bloque te da el indice en la tabla de arriba
		//y despues el numero % ese mismo divisor te da el indice adentro del bloque de la tabla

	}

	return 0;
}

void Ext2FS::read_block(unsigned int block_address, unsigned char * buffer)
{
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;
	for(unsigned int i = 0; i < sectors_per_block; i++)
		_hdd.read(blockaddr2sector(block_address)+i, buffer+i*SECTOR_SIZE);
	}

struct Ext2FSInode * Ext2FS::get_file_inode_from_dir_inode(struct Ext2FSInode * from, const char * filename)
{
	if(from == NULL)
		from = load_inode(EXT2_RDIR_INODE_NUMBER);
	//std::cerr << *from << std::endl;
	assert(INODE_ISDIR(from));
	
	//calculamos por enesima vez el tamaño del bloque, no es un global?
	unsigned int block_size = 1024 << _superblock->log_block_size;
	//preparo un buffer para trarme un bloque
	unsigned char * buffer = (unsigned char *) malloc(block_size*2);

	unsigned int cant_bloques = from->size / block_size; 
	unsigned int offset_dentro_del_bloque = 0;
	
//itero sobre todos los bloques del inodo
for (unsigned int i = 0; i < cant_bloques; i++) {
		//obtengo la adress del bloque i-esimo
		unsigned int block_address = get_block_address(from, i);
		unsigned int block_address2 = get_block_address(from,i+1);
		//lo cargo al buffer
		read_block(block_address, buffer);
		read_block(block_address2, buffer + block_size);
	


	//hay que ir "iterando" adentrod el bloque. porque no tenemos idea de cuantos hay
    while (offset_dentro_del_bloque < block_size) {
		//leo la dirEntry dentro del buffer
        Ext2FSDirEntry * dirEntryActual = (Ext2FSDirEntry*) (buffer + offset_dentro_del_bloque);

       //aca, una vez actualizado todo y toda la perorata, quiero ver que el nombre sea el

	   unsigned int largo_nombre = strlen(filename);
	   unsigned int largo_nombre2 = dirEntryActual->name_length;

	   unsigned int minimal = std::min(largo_nombre, largo_nombre2);
		
        if (largo_nombre2 == largo_nombre && !strncmp(filename, dirEntryActual->name, largo_nombre)) {
			//comparacion con el tamaño de la entrada actual? o 
			unsigned int inodoActual = dirEntryActual->inode;
            free(buffer);
            return load_inode(inodoActual);
        }

        // le sumo lo que ocupa esta dirEntry
        offset_dentro_del_bloque += dirEntryActual->record_length;
    }
	offset_dentro_del_bloque -= block_size;

}

	//podemos llegar aca? retorno null eso esta bien si no existe?
	return NULL;

}

/*
struct Ext2FSInode * Ext2FS::get_file_inode_from_dir_inode(struct Ext2FSInode * from, const char * filename)
{
	if(from == NULL)
		from = load_inode(EXT2_RDIR_INODE_NUMBER);
	//std::cerr << *from << std::endl;
	assert(INODE_ISDIR(from));
	
	//calculamos por enesima vez el tamaño del bloque, no es un global?
	unsigned int block_size = 1024 << _superblock->log_block_size;
	//preparo un buffer para trarme un bloque
	unsigned char * buffer = (unsigned char *) malloc(block_size*2);

	unsigned int cant_bloques = from->size / block_size; 
	unsigned int offset_dentro_del_bloque = 0;
	
//itero sobre todos los bloques del inodo
for (unsigned int i = 0; i < cant_bloques; i++) {
		//obtengo la adress del bloque i-esimo
		unsigned int block_address = get_block_address(from, i);
		unsigned int block_address2 = get_block_address(from,i+1);
		//lo cargo al buffer
		read_block(block_address, buffer);
		read_block(block_address2, buffer + block_size);
	


	//hay que ir "iterando" adentrod el bloque. porque no tenemos idea de cuantos hay
    while (offset_dentro_del_bloque < block_size) {
		//leo la dirEntry dentro del buffer
        Ext2FSDirEntry * dirEntryActual = (Ext2FSDirEntry*) (buffer + offset_dentro_del_bloque);

       //aca, una vez actualizado todo y toda la perorata, quiero ver que el nombre sea el

	   unsigned int largo_nombre = strlen(filename);
	   unsigned int largo_nombre2 = dirEntryActual->name_length;

	   unsigned int minimal = std::min(largo_nombre, largo_nombre2);
		
        if (largo_nombre2 == largo_nombre && !strncmp(filename, dirEntryActual->name, largo_nombre)) {
			//comparacion con el tamaño de la entrada actual? o 
			unsigned int inodoActual = dirEntryActual->inode;
            free(buffer);
            return load_inode(inodoActual);
        }

        // le sumo lo que ocupa esta dirEntry
        offset_dentro_del_bloque += dirEntryActual->record_length;
    }
	offset_dentro_del_bloque -= block_size;

}

	//podemos llegar aca? retorno null eso esta bien si no existe?
	return NULL;

}

*/

fd_t Ext2FS::get_free_fd()
{
	for(fd_t fd = 0; fd < EXT2_MAX_OPEN_FILES; fd++)
	{
		// Warning: This is inefficient
		if(!(_fd_status[fd/sizeof(unsigned int)] & (1 << (fd % sizeof(unsigned int)))))
			return fd;
	}
	return -1;
}

void Ext2FS::mark_fd_as_used(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] | (1 << (fd % sizeof(unsigned int))));
}

void Ext2FS::mark_fd_as_free(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] ^ (1 << (fd % sizeof(unsigned int))));
}

fd_t Ext2FS::open(const char * path, const char * mode)
{
	fd_t new_fd = get_free_fd();
	if(new_fd < 0)
		return -1;
	mark_fd_as_used(new_fd);

	// We ignore mode
	struct Ext2FSInode * inode = inode_for_path(path);

	if(inode == NULL)
		return -1;

	_open_files[new_fd] = *inode;
	delete inode;
	_seek_memory[new_fd] = 0;

	return new_fd;
}

int Ext2FS::read(fd_t filedesc, unsigned char * buffer, int size)
{
	int realsize = ((_seek_memory[filedesc] + size) >= _open_files[filedesc].size)?_open_files[filedesc].size-_seek_memory[filedesc]:size;
	if(realsize > 0)
	{
		int seek = _seek_memory[filedesc];
		unsigned int read = 0;

		// Compute block size by shifting the value 1024
		unsigned int block_size = 1024 << _superblock->log_block_size;
		unsigned int sectors_per_block = block_size / SECTOR_SIZE;

		unsigned int start_block = (seek / block_size);
		unsigned int end_block = ((seek + realsize - 1) / block_size);


		for(unsigned int block = start_block; block <= end_block; block++)
		{
			unsigned int block_address = get_block_address(&_open_files[filedesc], block);

			unsigned char block_buf[block_size];
			read_block(block_address, block_buf);

			do
			{
				buffer[read++] = block_buf[seek++ % block_size];
			} while(((seek % block_size) != 0) && (read < realsize));
		}
	}
	return realsize;
}

int Ext2FS::write(fd_t filedesc, const unsigned char * buffer, int size)
{
	return -1;
}

int Ext2FS::seek(fd_t filedesc, int offset)
{
	//std::cerr << "offset: " << offset << " size: " << _open_files[filedesc].size << std::endl;
	int position = offset;
	if(offset < 0)
		position = _open_files[filedesc].size + offset;
	if(position >= _open_files[filedesc].size)
		position = _open_files[filedesc].size - 1;
	_seek_memory[filedesc] = position;
	return position;
}

int Ext2FS::tell(fd_t filedesc)
{
	return _seek_memory[filedesc];
}

void Ext2FS::close(fd_t filedesc)
{
	mark_fd_as_free(filedesc);
}