#pragma once 

#include <memory>

#include "DescriptionBase.h"

class DescriptionFactoryBase {
public:
	DescriptionFactoryBase() = default;
	virtual ~DescriptionFactoryBase() = default;
	DescriptionFactoryBase& operator=(const DescriptionFactoryBase&) = delete;

	std::unique_ptr<DescriptionBase> create() {
		std::unique_ptr<DescriptionBase> created = _create();
		return created;
	}
protected:
	virtual std::unique_ptr<DescriptionBase> _create() = 0;
};


template<class TDescription>
class DescriptionFactory : DescriptionFactoryBase {
public:
	DescriptionFactory() = default;
	virtual ~DescriptionFactory() {}

protected:
	virtual std::unique_ptr<DescriptionBase> _create() override {
		return std::make_unique<TDescription>();
	}
};