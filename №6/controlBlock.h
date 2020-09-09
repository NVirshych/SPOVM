#pragma once

class controlBlock {

public:
	size_t size;
	bool used = false;

	controlBlock(size_t size) : size(size) {};

};