    // For deep property overrides
    void visitComponent(ComponentNode& node, const std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>>& pendingOverrides);
    void visit(ConditionalNode& node) override;
};
