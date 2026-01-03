#ifndef REPOSITORY_FACTORY_H
#define REPOSITORY_FACTORY_H

#include <memory>
#include "../repository/ProductRepository.h"  // Factory Pattern: Creates repository instances
#include "../database/DatabaseManager.h"

class RepositoryFactory {
public:
    // Factory Pattern: Create ProductRepository with injected DB manager
    static std::shared_ptr<ProductRepository> createProductRepository(DatabaseManager& dbManager) {
        return std::make_shared<ProductRepository>(dbManager);
    }
    // Add more for other entities (e.g., UserRepository) later
};

#endif  // REPOSITORY_FACTORY_H